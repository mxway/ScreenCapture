#include "FileDialog.h"
#include "PixmapCache.h"
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <libgen.h>

UIControl   *pFocusControl = nullptr;

FileDialog::FileDialog()
{
    //string path = ::getenv("HOME");
    char  selfFileName[1024] = {0};
    readlink("/proc/self/exe",selfFileName,1000);
    string path = dirname(selfFileName);
    m_fileList.SetPath(path);
    m_pathButton.SetPath(path);
    m_fileList.SetPathChangeNotify(this);
    m_pathButton.SetPathChangeNotify(this);
    m_fileList.SetWindowHandle(this->GetWindowHandle());
    m_pathButton.SetWindowHandle(this->GetWindowHandle());
    m_edit.SetWindowHandle(this->GetWindowHandle());
}

FileDialog::~FileDialog() {
    PixmapCache::GetInstance()->Clear(this->GetWindowHandle());
}

long FileDialog::OnReSize(MSG msg) {
    XBaseWindow::OnReSize(msg);
    X11Window  *window = this->GetWindowHandle();
    //UIRect  rect = {0,30,window->width,window->height-30};
    UIRect  rect = {30,30,window->width-60,window->height-100};
    m_fileList.SetRect(rect);
    UIRect  pathButtonRect = {30,0,window->width-60,30};
    m_pathButton.SetRect(pathButtonRect);
    UIRect  editRect = {30,window->height-60,window->width-60,40};
    m_edit.SetRect(editRect);
    this->Invalidate();
    return 0;
}

long FileDialog::OnPaint(MSG msg) {

    X11Window  *winHandle = this->GetWindowHandle();
    if(winHandle->offscreenPixmap != 0){
        XFreePixmap(winHandle->display,winHandle->offscreenPixmap);
    }
    winHandle->offscreenPixmap = XCreatePixmap(winHandle->display,winHandle->window,winHandle->width,winHandle->height,winHandle->depth);
    GC gc = XCreateGC(winHandle->display,winHandle->window,0, nullptr);
    XSetForeground(winHandle->display,gc, WhitePixel(winHandle->display,winHandle->screen));
    XFillRectangle(winHandle->display,winHandle->offscreenPixmap,gc,0,0,winHandle->width,winHandle->height);
    m_fileList.OnPaint(winHandle->offscreenPixmap);
    m_pathButton.OnPaint(winHandle->offscreenPixmap);
    m_edit.OnPaint(winHandle->offscreenPixmap);
    XCopyArea(winHandle->display,winHandle->offscreenPixmap,winHandle->window,gc,0,0,winHandle->width,winHandle->height,0,0);
    XFreeGC(winHandle->display,gc);
    return XBaseWindow::OnPaint(msg);
}

long FileDialog::OnMouseWheel(MSG msg) {
    m_fileList.OnMouserWheel(msg);
    return XBaseWindow::OnMouseWheel(msg);
}

long FileDialog::OnLeftButtonPress(MSG msg) {
    UIPoint point = {msg.xbutton.x,msg.xbutton.y};
    if(m_fileList.IsPointIn(point)){
        if(pFocusControl != &m_fileList){
            if(pFocusControl!= nullptr){
                pFocusControl->LostFocus();
            }
            pFocusControl = &m_fileList;
            pFocusControl->OnFocus();
        }
        m_fileList.OnLeftButtonPressed(msg);
    }else if(m_pathButton.IsPointIn(point)){
        if(pFocusControl != &m_pathButton){
            if(pFocusControl!= nullptr){
                pFocusControl->LostFocus();
            }
            pFocusControl = &m_pathButton;
            pFocusControl->OnFocus();
        }
        m_pathButton.OnLeftButtonPressed(msg);
    }else if(m_edit.IsPointIn(point)){
        if(pFocusControl != &m_edit){
            if(pFocusControl != nullptr){
                pFocusControl->LostFocus();
            }
            pFocusControl = &m_edit;
            pFocusControl->OnFocus();
        }
    }else{
        if(pFocusControl!= nullptr){
            pFocusControl->LostFocus();
            pFocusControl = nullptr;
        }
    }
    return XBaseWindow::OnLeftButtonPress(msg);
}

long FileDialog::OnLeftButtonRelease(MSG msg) {
    m_fileList.OnLeftMouseRelease(msg);
    return XBaseWindow::OnLeftButtonRelease(msg);
}

long FileDialog::OnMouseMove(MSG msg) {
    m_fileList.OnMouseMove(msg);
    return XBaseWindow::OnMouseMove(msg);
}

void FileDialog::PathChange(const string &path) {
    m_fileList.SetPath(path);
    m_pathButton.SetPath(path);
    this->Invalidate();
}

long FileDialog::OnKeyPress(MSG msg) {
    KeySym key = XLookupKeysym(&msg.xkey, 0);

    int modifiers = msg.xkey.state;
    if ((modifiers & ControlMask) && key == XK_c) {
        //printf("Ctrl + C\n");
        this->Close(UI_DLG_BUTTON_CLOSE);
        return 1;
    }
    if(pFocusControl != nullptr){
        pFocusControl->OnKeyDownEvent(msg);
    }
    //KeySym  key = XLookupKeysym(&msg.xkey,0);
    if(key == XK_Return){
        this->Close(UI_DLG_BUTTON_SAVE);
        return 0;
    }else if(key == XK_Escape){
        this->Close(UI_DLG_BUTTON_CANCEL);
        return 0;
    }
    return XBaseWindow::OnKeyPress(msg);
}

void FileDialog::Invalidate() {
    X11Window  *window = this->GetWindowHandle();
    XEvent event;
    event.type = Expose;
    event.xexpose.window = window->window;
    event.xexpose.x = 0;
    event.xexpose.y = 0;
    event.xexpose.width = window->width;
    event.xexpose.height = window->height;
    event.xexpose.count = 0;

    XSendEvent(window->display, window->window, False, ExposureMask, &event);
}

long FileDialog::OnClose(MSG msg) {
    return XBaseWindow::OnClose(msg);
}

long FileDialog::OnCreate() {
    X11Window  *window = this->GetWindowHandle();
    //UIRect  rect = {0,30,window->width,window->height-30};
    UIRect  rect = {30,30,window->width-60,window->height-100};
    m_fileList.SetRect(rect);
    UIRect  pathButtonRect = {30,0,window->width-60,30};
    m_pathButton.SetRect(pathButtonRect);
    UIRect  editRect = {30,window->height-60,window->width-60,40};
    m_edit.SetRect(editRect);
    this->ChangeWindowStyle();
    return XBaseWindow::OnCreate();
}

string FileDialog::GetFileName() {
    return m_fileList.GetPath() + "/" + m_edit.GetText();
}

void FileDialog::ChangeWindowStyle() {
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    XChangeWindowAttributes(m_x11Window->display, m_x11Window->window, CWOverrideRedirect, &attrs);
}
