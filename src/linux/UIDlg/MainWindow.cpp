#include <cstdio>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <cstdint>
#include <X11/extensions/Xrender.h>
#include <X11/cursorfont.h>
#include "MainWindow.h"
#include "DisplayInstance.h"
#include "FileDialog.h"
#include "Misc.h"
#include "../../ImageBitmapInfo.h"
#include "../../CImage.h"
#include "../../ImageFactory.h"
#include "../WindowBitmap.h"

MainWindow::MainWindow()
    : m_rootWindowPixmap{0},
      m_darkPixmap{0},
      m_gc{nullptr},
      m_selectionCursor{0}
{

}

MainWindow::~MainWindow() {
    if(m_rootWindowPixmap != 0){
        XFreePixmap(m_x11Window->display, m_rootWindowPixmap);
    }
    if(m_darkPixmap!=0){
        XFreePixmap(m_x11Window->display,m_darkPixmap);
    }
    if(m_gc != nullptr){
        XFreeGC(m_x11Window->display,m_gc);
    }
    XUngrabPointer(m_x11Window->display,CurrentTime);
    if(m_selectionCursor != 0){
        XFreeCursor(m_x11Window->display, m_selectionCursor);
        m_selectionCursor = 0;
    }

}

bool MainWindow::CreateFullScreenWindow() {
    if(DisplayInstance::GetInstance().GetDisplay() == nullptr){
        return false;
    }
    XBaseWindow::Create(0, 0, DisplayInstance::GetInstance().GetWidth(), DisplayInstance::GetInstance().GetHeight(), u8"ScreenCapture");
    return m_x11Window->window != 0;
}

long MainWindow::OnPaint(MSG msg) {
    XSetInputFocus(m_x11Window->display, m_x11Window->window,RevertToParent, CurrentTime);
    XCopyArea(m_x11Window->display,m_darkPixmap,m_x11Window->window,m_gc,0,0,m_x11Window->width,m_x11Window->height,0,0);
    return XBaseWindow::OnPaint(msg);
}

long MainWindow::OnCreate() {
    m_gc = XCreateGC(m_x11Window->display,m_x11Window->window,0, nullptr);
    m_selectionCursor = XCreateFontCursor(m_x11Window->display, XC_cross);
    XDefineCursor(m_x11Window->display, m_x11Window->window, m_selectionCursor);
    this->ChangeWindowStyle();
    this->CreateRootWindowPixmap();
    this->CreateDarkPixmap();
    XSelectInput(m_x11Window->display, m_x11Window->window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);
    return XBaseWindow::OnCreate();
}

long MainWindow::OnReSize(MSG msg) {
    return XBaseWindow::OnReSize(msg);
}

static bool glbLeftMousePressed = false;
static int mouseX = 0;
static int mouseY = 0;

long MainWindow::OnLeftButtonPress(MSG msg) {
    glbLeftMousePressed = 1;
    mouseX = msg.xbutton.x;
    mouseY = msg.xbutton.y;
    XCopyArea(m_x11Window->display,m_darkPixmap,m_x11Window->window,m_gc,0,0,m_x11Window->width,m_x11Window->height,0,0);
    return XBaseWindow::OnLeftButtonPress(msg);
}

UIRect oldRect = {-1,-1,0,0};

long MainWindow::OnLeftButtonRelease(MSG msg) {
    glbLeftMousePressed = 0;
    FileDialog  fileDialog;
    XUngrabPointer(m_x11Window->display,CurrentTime);
    fileDialog.Create(100,100,800,600,"Save File",m_x11Window->window,WindowType_Popup);
    uint32_t ret = fileDialog.ShowModal();
    if(ret == UI_DLG_BUTTON_SAVE){
        string savedFile = fileDialog.GetFileName();
        uint32_t imageType = 0;
        if(EndWith(savedFile,".bmp")){
            imageType = 1;
        }else if(EndWith(savedFile,".png")){
            imageType = 2;
        }else if(EndWith(savedFile,".jpg") || EndWith(savedFile,".jpeg")){
            imageType = 3;
        }else{
            savedFile.append(".bmp");
            imageType = 1;
        }
        ImageBitmapInfo imageBitmapInfo = GetImageBitmapInfoFromWindow(m_x11Window->display,m_rootWindowPixmap,oldRect);
        // GetImage
        // 1 -- save picture as  bmp file format
        // 2 -- save picture as png file format
        // 3 -- save picture as jpg file format
        CImage *image = CImageFactory::GetImage(imageType);
        image->saveImage(savedFile,imageBitmapInfo);
        free(imageBitmapInfo.data);
        delete image;
        this->Close(UI_DLG_BUTTON_OK);
    }
    return XBaseWindow::OnLeftButtonRelease(msg);
}

long MainWindow::OnKeyPress(MSG msg) {
    KeySym keysym = XLookupKeysym(&msg.xkey,0);
    if(keysym == XK_Escape){
        this->Close();
        return 0;
    }
    return XBaseWindow::OnKeyPress(msg);
}

long MainWindow::OnKeyRelease(MSG msg) {
    return XBaseWindow::OnKeyRelease(msg);
}

long MainWindow::OnClose(MSG msg) {
    return XBaseWindow::OnClose(msg);
}

long MainWindow::OnMouseMove(MSG msg) {
    if( !( (msg.xmotion.state & Button1Mask) && glbLeftMousePressed) ){
        return XBaseWindow::OnMouseMove(msg);
        return 0;
    }
    int startX = mouseX<msg.xbutton.x?mouseX:msg.xbutton.x;
    int startY = mouseY<msg.xbutton.y?mouseY:msg.xbutton.y;
    int tmpWidth = abs(mouseX-msg.xbutton.x);
    int tmpHeight = abs(mouseY-msg.xbutton.y);

    UIRect newRect = {startX,startY,tmpWidth,tmpHeight};

    XSetForeground(m_x11Window->display,m_gc, 0xFFFF0000);
    char dash_list[] = {10,20};
    int dash_offset = 0;
    XSetDashes(m_x11Window->display,m_gc,dash_offset,dash_list,2);

    XSetLineAttributes(m_x11Window->display,m_gc,1,LineOnOffDash,CapButt,JoinMiter);

    UIRect diffRectArray[4] = {0};
    int count = 0;
    rect_difference(&newRect, &oldRect,diffRectArray,&count);
    for(auto & diffRect : diffRectArray){
        if(diffRect.width <= 0 || diffRect.height <= 0){
            continue;
        }
        XCopyArea(m_x11Window->display, m_rootWindowPixmap, m_x11Window->window, m_gc, diffRect.x, diffRect.y,
                  diffRect.width + 1, diffRect.height + 1, diffRect.x, diffRect.y);
    }
    memset(diffRectArray,0,sizeof(diffRectArray));
    rect_difference(&oldRect,&newRect,diffRectArray,&count);
    for(auto & rangeRect : diffRectArray){
        if(rangeRect.width <= 0 || rangeRect.height <= 0){
            continue;
        }
        XCopyArea(m_x11Window->display, m_darkPixmap, m_x11Window->window, m_gc, rangeRect.x, rangeRect.y,
                  rangeRect.width + 1, rangeRect.height + 1, rangeRect.x, rangeRect.y);
    }
    oldRect = newRect;
    XCopyArea(m_x11Window->display,m_rootWindowPixmap,m_x11Window->window,m_gc,oldRect.x,oldRect.y,1,oldRect.height,oldRect.x,oldRect.y);
    XCopyArea(m_x11Window->display,m_rootWindowPixmap,m_x11Window->window,m_gc,oldRect.x,oldRect.y,oldRect.width,1,oldRect.x,oldRect.y);
    XCopyArea(m_x11Window->display,m_rootWindowPixmap,m_x11Window->window,m_gc, oldRect.x, oldRect.y+oldRect.height,oldRect.width,1,oldRect.x,oldRect.y+oldRect.height);
    XCopyArea(m_x11Window->display,m_rootWindowPixmap,m_x11Window->window,m_gc, oldRect.x+oldRect.width,oldRect.y,1,oldRect.height,oldRect.x+oldRect.width,oldRect.y);
    XDrawRectangle(m_x11Window->display,m_x11Window->window,m_gc, oldRect.x,oldRect.y,oldRect.width,oldRect.height);
    return 0;
    //return XBaseWindow::OnMouseMove(msg);
}

long MainWindow::OnMouseWheel(MSG msg) {
    return XBaseWindow::OnMouseWheel(msg);
}

long MainWindow::OnRightButtonPress(MSG msg) {
    this->Close(UI_DLG_BUTTON_CANCEL);
    return 0;
    //return XBaseWindow::OnRightButtonPress(msg);
}

void MainWindow::ChangeWindowStyle() {
    Atom wm_type = XInternAtom(m_x11Window->display, "_NET_WM_WINDOW_TYPE", False);
    Atom wm_type_dock = XInternAtom(m_x11Window->display, "_NET_WM_WINDOW_TYPE_DOCK", False);
    XChangeProperty(m_x11Window->display, m_x11Window->window, wm_type, XA_ATOM, 32, PropModeReplace,
                    (unsigned char *)&wm_type_dock, 1);

    // Disable window decorate,we will create a window without title bar and border
    Atom wm_state = XInternAtom(m_x11Window->display, "_NET_WM_STATE", False);
    Atom wm_state_skip_taskbar = XInternAtom(m_x11Window->display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    Atom wm_state_skip_pager = XInternAtom(m_x11Window->display, "_NET_WM_STATE_SKIP_PAGER", False);
    XChangeProperty(m_x11Window->display, m_x11Window->window, wm_state, XA_ATOM, 32, PropModeReplace,
                    (unsigned char *)&wm_state_skip_taskbar, 1);
    XChangeProperty(m_x11Window->display, m_x11Window->window, wm_state, XA_ATOM, 32, PropModeAppend,
                    (unsigned char *)&wm_state_skip_pager, 1);

    // setting the window to top level
    Atom wm_state_above = XInternAtom(m_x11Window->display, "_NET_WM_STATE_ABOVE", False);
    XChangeProperty(m_x11Window->display, m_x11Window->window, wm_state, XA_ATOM, 32, PropModeAppend,
                    (unsigned char *)&wm_state_above, 1);

    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    XChangeWindowAttributes(m_x11Window->display, m_x11Window->window, CWOverrideRedirect, &attrs);
}

void MainWindow::CreateRootWindowPixmap() {
    if(m_rootWindowPixmap != 0){
        XFreePixmap(DisplayInstance::GetInstance().GetDisplay(), m_rootWindowPixmap);
        m_rootWindowPixmap = 0;
    }
    m_rootWindowPixmap = XCreatePixmap(m_x11Window->display, m_x11Window->window, m_x11Window->width, m_x11Window->height, m_x11Window->depth);
    XImage *rootImage = XGetImage(m_x11Window->display, RootWindow(m_x11Window->display,m_x11Window->screen),0,0,m_x11Window->width,m_x11Window->height,AllPlanes,ZPixmap);

    XPutImage(m_x11Window->display, m_rootWindowPixmap, m_gc, rootImage, 0, 0, 0, 0, m_x11Window->width, m_x11Window->height);
    XDestroyImage(rootImage);
}

void MainWindow::CreateDarkPixmap() {
    m_darkPixmap = XCreatePixmap(m_x11Window->display,m_x11Window->window, m_x11Window->width,m_x11Window->height, m_x11Window->depth);
    Pixmap blackBackground = XCreatePixmap(m_x11Window->display, m_x11Window->window,m_x11Window->width,m_x11Window->height, 32);
    XRenderPictFormat *format {nullptr};
    if(m_x11Window->depth == 32){
        format = XRenderFindStandardFormat(m_x11Window->display, PictStandardARGB32);
    }else{
        format = XRenderFindStandardFormat(m_x11Window->display, PictStandardRGB24);
    }

    XRenderColor color = {0x0,0x0,0x0,0x8000};
    Picture blackBackgroundPicture = XRenderCreatePicture(m_x11Window->display,blackBackground,
                                                          XRenderFindStandardFormat(m_x11Window->display, PictStandardARGB32),0,nullptr);
    XRenderFillRectangle(m_x11Window->display,PictOpSrc,blackBackgroundPicture,&color,0,0,m_x11Window->width,m_x11Window->height);

    Picture picture = XRenderCreatePicture(m_x11Window->display,m_rootWindowPixmap, format,0, nullptr);
    Picture window_picture = XRenderCreatePicture(m_x11Window->display,m_darkPixmap,format,0,nullptr);

    XRenderComposite(m_x11Window->display,PictOpOver,picture, None,window_picture,0,0,0,0,0,0,m_x11Window->width,m_x11Window->height);
    XRenderComposite(m_x11Window->display,PictOpOver,blackBackgroundPicture, None,window_picture,0,0,0,0,0,0,m_x11Window->width,m_x11Window->height);

    XRenderFreePicture(m_x11Window->display,blackBackgroundPicture);
    XRenderFreePicture(m_x11Window->display,window_picture);
    XRenderFreePicture(m_x11Window->display, picture);
    //XFreeGC(m_x11Window->display,gc);
    XFreePixmap(m_x11Window->display,blackBackground);
}
