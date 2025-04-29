#include "UIEdit.h"
#include <X11/Xft/Xft.h>

typedef struct _ICData
{
    XIC m_xic;
    XIM m_xim;
}ICData;

const int CURSOR_WIDTH = 6;
const int CURSOR_HEIGHT=24;

static unsigned char cursoricon_bits [ ] = {
        0x3f , 0x03f , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c ,
        0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x0c , 0x03f , 0x03f } ;

UIEdit::UIEdit()
    :m_ic{new ICData },
    m_font{nullptr},
    m_cursor{0},
    m_focusState{false}
{
    ((ICData*)m_ic)->m_xic = nullptr;
    ((ICData*)m_ic)->m_xic = nullptr;
}

UIEdit::~UIEdit() {
    ICData  *data = (ICData*)m_ic;
    if(data && data->m_xic){
        XUnsetICFocus(data->m_xic);
        XDestroyIC(data->m_xic);
    }
    if(data && data->m_xim){
        XCloseIM(data->m_xim);
    }
    X11Window  *window = this->GetWindowHandle();
    if(m_cursor != 0){
        XFreePixmap(window->display,m_cursor);
    }
    if(m_font!= nullptr){
        XftFontClose(window->display,m_font);
        m_font = nullptr;
    }
    delete data;
}

void UIEdit::OnPaint(HDC hdc) {
    UIControl::OnPaint(hdc);

    X11Window *window = this->GetWindowHandle();
    GC gc = XCreateGC(window->display,hdc, 0, nullptr);
    XftDraw *xftDraw = XftDrawCreate(window->display,hdc, window->visual,window->colormap);
    XSetForeground(window->display,gc, 0xff888888);
    XSetLineAttributes(window->display,gc,2,LineSolid,CapButt,JoinRound);
    XDrawRectangle(window->display,hdc, gc, m_rect.x,m_rect.y, m_rect.width,m_rect.height);
    XSetForeground(window->display,gc, 0xffffffff);
    XFillRectangle(window->display,hdc,gc,m_rect.x+2,m_rect.y+2, m_rect.width-3,m_rect.height-3);
    if(m_font == nullptr){
        m_font = XftFontOpenName(window->display,window->screen,"default:size=12");
    }

    XftColor color;
    color.color.alpha = 0xffff;
    color.color.red = 0;
    color.color.green = 0;
    color.color.blue = 0;
    if(m_text.length()>0){
        XftDrawStringUtf8(xftDraw, &color, m_font,m_rect.x+3,m_rect.y+m_font->ascent+5,
                          reinterpret_cast<const FcChar8 *>(m_text.c_str()), m_text.length());
    }

    this->DrawInsertionCursor(hdc);

    XftDrawDestroy(xftDraw);
    XFreeGC(window->display,gc);
}

void UIEdit::OnFocus() {
    UIControl::OnFocus();
    X11Window *window = this->GetWindowHandle();
    ICData *data = (ICData*)m_ic;
    if(data->m_xim == nullptr){
        XSetLocaleModifiers("");
        data->m_xim = XOpenIM(window->display,0,0,0);
    }
    if(data->m_xic == nullptr){
        data->m_xic = XCreateIC(data->m_xim,XNInputStyle,XIMPreeditNothing|XIMStatusNothing,
                                XNClientWindow,window->window,
                                XNFocusWindow,window->window,nullptr);
    }
    m_focusState = true;
    XSetICFocus(data->m_xic);
    this->Invalidate();
}

void UIEdit::LostFocus() {
    UIControl::LostFocus();
    ICData *data = (ICData*)m_ic;
    m_focusState = false;
    XUnsetICFocus(data->m_xic);
    this->Invalidate();
}

void set_preedit_position(XIC ic, int x, int y) {
    XPoint point = {static_cast<short>(x),static_cast<short>(y)};
    XVaNestedList preedit_attr = XVaCreateNestedList(0,
                                                     XNSpotLocation, &point,
                                                     NULL);
    if (preedit_attr) {
        XSetICValues(ic, XNPreeditAttributes, preedit_attr, NULL);
        XFree(preedit_attr);
    }
}

static int GetTextWidth(Display *display,XftFont *font,const string &str)
{
    XGlyphInfo  glyphInfo{0};
    ::XftTextExtentsUtf8(display, font, reinterpret_cast<const FcChar8 *>(str.c_str()), str.length(), &glyphInfo);
    return glyphInfo.width;
}

static bool IsPrintableChar(KeySym keysym)
{
    return (keysym>=0x20 && keysym<127) || (keysym>=XK_KP_Multiply && keysym<=XK_KP_9);
}

void UIEdit::OnKeyDownEvent(MSG msg) {
    UIControl::OnKeyDownEvent(msg);
    Status status;
    KeySym keysym = NoSymbol;
    ICData *data = (ICData*)m_ic;
    X11Window  *window = this->GetWindowHandle();
    int textWidth = GetTextWidth(window->display,m_font,m_text);
    set_preedit_position(data->m_xic,m_rect.x + textWidth,m_rect.y + m_rect.height);
    char text[32] = {};
    /*int is_repeat = prev_ev.type == KeyRelease &&
                    prev_ev.xkey.time == ev.xkey.time &&
                    prev_ev.xkey.keycode == ev.xkey.keycode;*/

    //msg.xkey.state &= ~ControlMask;
    Xutf8LookupString(data->m_xic,&msg.xkey,text,sizeof(text)-1,&keysym,&status);
    if(status == XBufferOverflow){
        //an IME was probably used,and wants to commit more than 32 chars.
        //ignore this fairly unlikely case for now
    }
    if(status == XLookupChars){
        m_text.append(text);
        this->Invalidate();
    }
    if(status == XLookupBoth){
        if( (!(msg.xkey.state & ControlMask)) && IsPrintableChar(keysym))
        {
            m_text.append(text);
            this->Invalidate();
        }
        if(keysym == XK_BackSpace){
            if(m_text.length()==0){
                return;
            }
            const char *p = m_text.c_str() + m_text.length();
            const char *charStart = CharPrev(m_text.c_str(),p);
            m_text.erase(charStart - m_text.c_str(), p-charStart);
            this->Invalidate();
        }
        //char *sym_name = XKeysymToString(keysym);
    }
    if(status == XLookupKeySym){
        //a key without text on it
    }
}

void UIEdit::DrawInsertionCursor(HDC hdc) {
    if(!m_focusState){
        return;
    }
    X11Window  *window = this->GetWindowHandle();
    if(m_cursor == 0){
        m_cursor = XCreatePixmapFromBitmapData( window->display , window->window ,
                reinterpret_cast<char *>(cursoricon_bits), CURSOR_WIDTH , CURSOR_HEIGHT ,
                0xff000000 , WhitePixel ( window->display , window->screen ) ,
                window->depth ) ;
    }
    int textWidth = GetTextWidth(window->display,m_font,m_text);
    GC gc = XCreateGC(window->display,hdc,0, nullptr);
    XCopyArea(window->display,m_cursor,hdc,gc,0,0,CURSOR_WIDTH,CURSOR_HEIGHT,m_rect.x + textWidth+2,m_rect.y + 8);
    XFreeGC(window->display,gc);
}

