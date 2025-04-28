#include "UIFileListControl.h"
#include <dirent.h>
#include <X11/Xft/Xft.h>
#include "IconPixmap.h"
#include "Misc.h"

static uint32_t maxDrawItems = 0;
const static int SCROLLBAR_WIDTH=16;
static int mouseY = 0;
static bool mousePressed = false;
static int oldStartDrawItem=0;

static bool ShouldPaintScrollBar(size_t totalFiles){
    return totalFiles>maxDrawItems;
}

UIFileListControl::UIFileListControl()
    :m_curSel{-1},
    m_pathChangeNotify{nullptr},
    m_startDrawItem{0},
    m_itemHeight{1},
    m_thumbRect{0,0,1,1}
{

}

static string GetFileName(const string &fullFileName)
{
    string::size_type pos = fullFileName.rfind('/');
    if(pos == string::npos){
        return fullFileName;
    }
    return fullFileName.substr(pos+1);
}

void UIFileListControl::OnPaint(HDC hdc) {
    UIControl::OnPaint(hdc);
    this->PaintFileList(hdc);
    this->PaintScrollBar(hdc);
}

static unsigned long LastClickTime = 0;
const int DOUBLE_CLICK_INTERVAL = 300;
void UIFileListControl::OnLeftButtonPressed(MSG msg) {
    UIPoint point = {msg.xbutton.x,msg.xbutton.y};
    if(!IsPointIn(point)){
        return;
    }
    if(ShouldPaintScrollBar(m_files.size())){
        //click file list select an item.
        this->DolFileSelectionEvent(msg);
        this->DoScrollBarEvent(msg);
    }else{
        this->DolFileSelectionEvent(msg);
    }
}

void UIFileListControl::OnLeftMouseRelease(MSG msg) {
    mousePressed = false;
}

void UIFileListControl::OnMouserWheel(MSG msg) {
    UIPoint  point = {msg.xbutton.x,msg.xbutton.y};
    if( !IsPointIn(point)){
        return;
    }
    if(msg.xbutton.button == Button4){
        this->ScrollUp();
        return;
    }
    this->ScrollDown();
}

void UIFileListControl::OnMouseMove(MSG msg) {
    if(!mousePressed){
        return;
    }
    double hideItemHeight = (m_rect.height-2*SCROLLBAR_WIDTH)*1.0/m_files.size();
    int deltaItem = (msg.xbutton.y_root - mouseY)/hideItemHeight;
    if(m_startDrawItem == oldStartDrawItem+deltaItem){
        return;
    }
    m_startDrawItem = oldStartDrawItem + deltaItem;
    if(m_startDrawItem<0){
        m_startDrawItem = 0;
    }
    if(m_startDrawItem>=m_files.size()-maxDrawItems){
        m_startDrawItem = m_files.size()-maxDrawItems;
    }
    this->Invalidate();
}

void UIFileListControl::DoDoubleClick(MSG msg) {
    if(m_curSel<0 || m_curSel>=m_files.size()){
        return;
    }
    string fileName = m_files[m_curSel];
    if(IsDirectory(fileName)){
        m_curSel = -1;
        if(m_pathChangeNotify != nullptr){
            m_pathChangeNotify->PathChange(fileName);
        }
    }
}

void UIFileListControl::DolFileSelectionEvent(MSG msg) {
    UIPoint  point = {msg.xbutton.x,msg.xbutton.y};
    //click file list select an item.
    bool clickOnFileList = ShouldPaintScrollBar(m_files.size())
            ?point.x>=m_rect.x && point.x<=m_rect.x+m_rect.width - SCROLLBAR_WIDTH:
                           point.x>=m_rect.x && point.x<=m_rect.x+m_rect.width;
    if(clickOnFileList){
        m_curSel = m_startDrawItem + (point.y-m_rect.y)/m_itemHeight;
        if(msg.xbutton.time - LastClickTime < DOUBLE_CLICK_INTERVAL){
            //double click...
            LastClickTime = 0;
            this->DoDoubleClick(msg);
            return;
        }
        LastClickTime = msg.xbutton.time;
        this->Invalidate();
    }
}

void UIFileListControl::DoScrollBarEvent(MSG msg) {
    UIPoint point = {msg.xbutton.x, msg.xbutton.y};
    if(point.x>=m_rect.x+m_rect.width-SCROLLBAR_WIDTH && point.y<=m_rect.y+SCROLLBAR_WIDTH){
        //click scrollbar up arrow button
        this->ScrollUp();
    }else if(point.x>=m_rect.x + m_rect.width-SCROLLBAR_WIDTH && point.y>=m_rect.height-SCROLLBAR_WIDTH){
        //click scrollbar down arrow button
        this->ScrollDown();
    }else if(point.x>=m_rect.x + m_rect.width-SCROLLBAR_WIDTH && point.y<m_thumbRect.y){
        //click above the thumb
        this->ScrollUp(3);
    }else if(point.x>=m_rect.x + m_rect.width-SCROLLBAR_WIDTH && point.y>m_thumbRect.y+m_thumbRect.height ){
        //click below the thumb
        this->ScrollDown(3);
    }else if(point.x>=m_rect.x + m_rect.width-SCROLLBAR_WIDTH && point.y>=m_thumbRect.y && point.y<=m_thumbRect.y+m_thumbRect.height){
        //click on the thumb
        mousePressed = true;
        mouseY = msg.xbutton.y_root;
        oldStartDrawItem = m_startDrawItem;
    }
}

void UIFileListControl::SetPath(const string &path) {
    if(path == m_currentDir){
        return;
    }
    m_currentDir = path;
    m_curSel = -1;
    m_startDrawItem = 0;
    this->LoadSubFiles();
}

static GC CreateImageGC(X11Window *window, Pixmap pixmap,UIRect rect){
    GC imageGC = XCreateGC(window->display, pixmap,0, nullptr);
    XSetForeground(window->display,imageGC,0);
    XFillRectangle(window->display,pixmap,imageGC,0,0,rect.width,rect.height);
    return imageGC;
}

static XftColor GetWhiteColor(){
    XftColor color;
    color.color.alpha = 0xffff;
    color.color.red = 0xffff;
    color.color.green = 0xffff;
    color.color.blue = 0xffff;
    return color;
}

static XftColor GetBlackColor(){
    XftColor color;
    color.color.alpha = 0xffff;
    color.color.red = 0;
    color.color.green = 0;
    color.color.blue = 0;
    return color;
}

const int ICON_WIDTH = 24;

void UIFileListControl::PaintFileList(HDC hdc) {
    X11Window *window = this->GetWindowHandle();
    XftFont *font = XftFontOpenName(window->display, window->screen,"Default:size=14");
    maxDrawItems = (m_rect.height)/(font->height+10);
    if(m_startDrawItem>0 && m_startDrawItem + maxDrawItems > m_files.size()){
        m_startDrawItem = m_files.size() - maxDrawItems;
        if(m_startDrawItem<0){
            m_startDrawItem = 0;
        }
    }

    m_itemHeight =  font->height + 10;
    XftDraw *draw = XftDrawCreate(window->display, hdc, window->visual,
                                  window->colormap);
    Pixmap offscreenImagePixmap = XCreatePixmap(window->display,window->window,m_rect.width,m_rect.height,32);
    GC imageGC = CreateImageGC(window,offscreenImagePixmap,m_rect);
    GC textGC = XCreateGC(window->display,hdc,0,nullptr);
    XSetForeground(window->display,textGC,0xffeeeeee);
    XFillRectangle(window->display,hdc, textGC,m_rect.x, m_rect.y,m_rect.width,m_rect.height);

    for(int i=0;i<maxDrawItems && i<m_files.size();i++){
        string fileName = GetFileName(m_files[i+m_startDrawItem]);
        if(i+m_startDrawItem==m_curSel){
            XftColor whiteColor = GetWhiteColor();
            XSetForeground(window->display,textGC,0xff007aff);
            // draw selected item background
            XFillRectangle(window->display,hdc,textGC,m_rect.x,i*m_itemHeight+m_rect.y,m_rect.width,m_itemHeight);
            XftDrawStringUtf8(draw, &whiteColor, font, m_rect.x + 40,i*m_itemHeight + font->ascent + m_rect.y + 5,
                              reinterpret_cast<const FcChar8 *>(fileName.c_str()), fileName.length());
        }else{
            XftColor blackColor = GetBlackColor();
            XftDrawStringUtf8(draw, &blackColor, font, m_rect.x + 40,i*m_itemHeight + font->ascent + m_rect.y + 5,
                              reinterpret_cast<const FcChar8 *>(fileName.c_str()), fileName.length());
        }

        Pixmap  pixmap = CreateIconPixmap(window->display,window->window,ICON_WIDTH,ICON_WIDTH,m_files[i+m_startDrawItem].c_str());
        XCopyArea(window->display,pixmap,offscreenImagePixmap,imageGC,0,0,ICON_WIDTH,ICON_WIDTH,8,i*m_itemHeight+8);
    }
    Picture imagePicture = XRenderCreatePicture(window->display, offscreenImagePixmap, XRenderFindStandardFormat(window->display, PictStandardARGB32),
                                               0, nullptr);
    Picture windowPicture = XRenderCreatePicture(window->display,hdc, XRenderFindStandardFormat(window->display, PictStandardRGB24),
                                                 0, nullptr);

    XRenderComposite(window->display,PictOpOver,imagePicture,None,windowPicture,0,0,0,0,m_rect.x,m_rect.y,m_rect.width,m_rect.height);
    XRenderFreePicture(window->display,windowPicture);
    XRenderFreePicture(window->display,imagePicture);
    XFreeGC(window->display,textGC);
    XFreeGC(window->display,imageGC);
    XFreePixmap(window->display,offscreenImagePixmap);
    XftDrawDestroy(draw);
    XftFontClose(window->display,font);
}

void UIFileListControl::PaintScrollBar(HDC hdc) {
    if(!ShouldPaintScrollBar(m_files.size())){
        return;
    }
    X11Window  *window = this->GetWindowHandle();
    GC scrollBarGC = XCreateGC(window->display,hdc, 0, nullptr);

    XSetForeground(window->display,scrollBarGC, 0xffe8e8e8);
    XFillRectangle(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-SCROLLBAR_WIDTH,m_rect.y,SCROLLBAR_WIDTH,SCROLLBAR_WIDTH);
    XFillRectangle(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-SCROLLBAR_WIDTH,m_rect.y + m_rect.height-SCROLLBAR_WIDTH,SCROLLBAR_WIDTH,SCROLLBAR_WIDTH);

    XSetForeground(window->display,scrollBarGC,0xffBBBBBB);
    XDrawRectangle(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-SCROLLBAR_WIDTH,m_rect.y,SCROLLBAR_WIDTH,SCROLLBAR_WIDTH);
    XDrawRectangle(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-SCROLLBAR_WIDTH,m_rect.y + m_rect.height-SCROLLBAR_WIDTH,SCROLLBAR_WIDTH,SCROLLBAR_WIDTH);
    XSetForeground(window->display,scrollBarGC, 0xff2c2c2c);
    XSetLineAttributes(window->display,scrollBarGC,2,LineSolid,CapButt,JoinRound);

    //draw up arrow
    XDrawLine(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-12,m_rect.y + 12,m_rect.x + m_rect.width-8,m_rect.y + 6);
    XDrawLine(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-8,m_rect.y  + 6,m_rect.x + m_rect.width-4,m_rect.y+12);
    //draw down arrow
    XDrawLine(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-12,m_rect.y+m_rect.height-12,m_rect.x + m_rect.width-8,m_rect.y+m_rect.height-6);
    XDrawLine(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-8,m_rect.y + m_rect.height-6,m_rect.x + m_rect.width-4,m_rect.y + m_rect.height-12);

    XSetForeground(window->display,scrollBarGC, 0xffEDF1F4);
    XFillRectangle(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-SCROLLBAR_WIDTH,m_rect.y + SCROLLBAR_WIDTH,SCROLLBAR_WIDTH,m_rect.height-SCROLLBAR_WIDTH*2);
    XSetForeground(window->display,scrollBarGC, 0xffcccccc);
    XSetLineAttributes(window->display,scrollBarGC,1,LineSolid,CapButt,JoinRound);
    XDrawRectangle(window->display,hdc,scrollBarGC,m_rect.x + m_rect.width-SCROLLBAR_WIDTH,m_rect.y+SCROLLBAR_WIDTH,SCROLLBAR_WIDTH,m_rect.height-SCROLLBAR_WIDTH*2);
    //draw bar
    double hideItemHeight = (m_rect.height-2*SCROLLBAR_WIDTH)*1.0/m_files.size();

    XSetForeground(window->display,scrollBarGC, 0xffc4c4c4);
    m_thumbRect = {m_rect.x + m_rect.width-SCROLLBAR_WIDTH,static_cast<int>(m_rect.y + SCROLLBAR_WIDTH + 1+m_startDrawItem*hideItemHeight),SCROLLBAR_WIDTH,static_cast<int>(m_rect.height-SCROLLBAR_WIDTH*2-hideItemHeight*(m_files.size()-maxDrawItems))};
    XDrawRectangle(window->display,hdc,scrollBarGC, m_thumbRect.x,m_thumbRect.y,m_thumbRect.width,m_thumbRect.height);
    XSetForeground(window->display,scrollBarGC, 0xfff8f8f8);
    XFillRectangle(window->display,hdc,scrollBarGC,m_thumbRect.x+1,m_thumbRect.y+1,SCROLLBAR_WIDTH-1,m_thumbRect.height-1);
    XFreeGC(window->display,scrollBarGC);

}

void UIFileListControl::LoadSubFiles() {
    m_files.clear();
    DIR *dir = opendir(m_currentDir.c_str());
    struct dirent *entry=nullptr;
    while( (entry = readdir(dir)) && entry!= nullptr){
        if(entry->d_name[0]=='.'){
            continue;
        }
        m_files.push_back(m_currentDir + "/" + entry->d_name);
    }
    ::closedir(dir);
}

void UIFileListControl::ScrollUp(int step) {
    if(m_startDrawItem==0){
        return;
    }
    m_startDrawItem = m_startDrawItem - step;
    if(m_startDrawItem <0){
        m_startDrawItem = 0;
    }
    this->Invalidate();
}

void UIFileListControl::ScrollDown(int step) {
    if(m_startDrawItem == m_files.size()-maxDrawItems){
        return;
    }
    m_startDrawItem += step;
    if(m_startDrawItem >= m_files.size()-maxDrawItems){
        m_startDrawItem = m_files.size()- maxDrawItems;
    }
    this->Invalidate();
}

void UIFileListControl::OnFocus() {
    UIControl::OnFocus();
}

void UIFileListControl::LostFocus() {
    m_curSel = -1;
    UIControl::LostFocus();
    this->Invalidate();
}

string UIFileListControl::GetPath() const {
    if(m_curSel<0 || m_curSel>=m_files.size()){
        return m_currentDir;
    }
    return m_currentDir + "/" + m_files[m_curSel];
}
