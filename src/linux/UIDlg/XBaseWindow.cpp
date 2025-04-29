#include "XBaseWindow.h"
#include <cstring>
#include <cstdio>
#include <X11/keysym.h>
#include <locale.h>
#include <X11/Xatom.h>
#include "DisplayInstance.h"

XBaseWindow::XBaseWindow()
{
    m_x11Window = new X11Window;
    memset(m_x11Window,0,sizeof(X11Window));
}

XBaseWindow::~XBaseWindow() {
    if(m_x11Window == nullptr){
        return;
    }
    if(m_x11Window->display!=nullptr && m_x11Window->window != 0){
        XDestroyWindow(m_x11Window->display, m_x11Window->window);
    }
    delete m_x11Window;
}

bool XBaseWindow::Create(int x, int y, int width, int height, const char *title, Window parent,WindowType windowType) {
    setlocale(LC_ALL, "");
    if(DisplayInstance::GetInstance().GetDisplay() == nullptr){
        return false;
    }
    m_x11Window->display = DisplayInstance::GetInstance().GetDisplay();
    m_x11Window->screen = DisplayInstance::GetInstance().GetScreenNumber();
    m_x11Window->depth = DisplayInstance::GetInstance().GetDisplayDepth();
    m_x11Window->visual = DisplayInstance::GetInstance().GetVisual();
    m_x11Window->colormap = DisplayInstance::GetInstance().GetColormap();
    if(m_x11Window->window != 0){
        fprintf(stderr,"Window Already been Created...\n");
        return false;
    }
    m_x11Window->window = XCreateSimpleWindow(m_x11Window->display, RootWindow(m_x11Window->display,m_x11Window->screen),
                                              x,y,width,height,1,BlackPixel(m_x11Window->display, m_x11Window->screen),
                                              WhitePixel(m_x11Window->display, m_x11Window->screen));
    m_x11Window->x = x;
    m_x11Window->y = y;
    m_x11Window->width = width;
    m_x11Window->height = height;

    if(m_x11Window== 0){
        return false;
    }
    this->OnCreate();
    this->SetTitle(title);
    if(windowType != WindowType_TopLevel){
        XSetTransientForHint(m_x11Window->display,m_x11Window->window,parent);
    }
    XSelectInput(m_x11Window->display, m_x11Window->window, ExposureMask | KeyPressMask | KeyReleaseMask
                            |ButtonReleaseMask | ButtonPressMask | ButtonMotionMask | StructureNotifyMask);
    return true;
}

void XBaseWindow::ShowWindow() {
    ::XMapWindow(m_x11Window->display,m_x11Window->window);
}

static int modalRetValue = UI_DLG_BUTTON_CLOSE;

static void set_modal_hint(Display *display, Window window) {
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom modal = XInternAtom(display, "_NET_WM_STATE_MODAL", False);

    XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace,
                    (unsigned char *)&modal, 1);
}

int XBaseWindow::ShowModal() {
    set_modal_hint(m_x11Window->display,m_x11Window->window);
    XMapWindow(m_x11Window->display,m_x11Window->window);
    XEvent event;

    Atom wm_delete_window = XInternAtom(m_x11Window->display, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(m_x11Window->display, m_x11Window->window, &wm_delete_window, 1);
    bool running = true;
    while (running) {
        XNextEvent(m_x11Window->display, &event);

        if(XFilterEvent(&event,None)){
            continue;
        }
        if(event.type == DestroyNotify){
            this->OnFinalMessage();
            running = false;
            continue;
        }
        this->MessageHandler(event);
    }
    return modalRetValue;
}

void XBaseWindow::SetTitle(const char *title) {
    Atom net_wm_name = XInternAtom(m_x11Window->display, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(m_x11Window->display, "UTF8_STRING", False);

    XChangeProperty(m_x11Window->display, m_x11Window->window, net_wm_name, utf8_string, 8,
                    PropModeReplace, (unsigned char *)title, strlen(title));
}

static bool glbContinueRunning = true;

void XBaseWindow::MessageLoop() {
    XEvent event;

    Atom wm_delete_window = XInternAtom(m_x11Window->display, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(m_x11Window->display, m_x11Window->window, &wm_delete_window, 1);

    while (glbContinueRunning) {
        XNextEvent(m_x11Window->display, &event);
        if(XFilterEvent(&event,None)){
            continue;
        }
        this->MessageHandler(event);
    }
}

void XBaseWindow::OnFinalMessage() {

}

long XBaseWindow::OnPaint(MSG msg) {
    return 0;
}

long XBaseWindow::OnCreate() {
    return 0;
}

long XBaseWindow::OnReSize(MSG msg) {
    m_x11Window->x = msg.xconfigure.x;
    m_x11Window->y = msg.xconfigure.y;
    m_x11Window->width = msg.xconfigure.width;
    m_x11Window->height = msg.xconfigure.height;
    return 0;
}

long XBaseWindow::OnLeftButtonPress(MSG msg) {
    return 0;
}

long XBaseWindow::OnLeftButtonRelease(MSG msg) {
    return 0;
}

long XBaseWindow::OnRightButtonPress(MSG msg) {
    return 0;
}

long XBaseWindow::OnRightButtonRelease(MSG msg) {
    return 0;
}

long XBaseWindow::OnKeyPress(MSG msg) {
    return 0;
}

long XBaseWindow::OnKeyRelease(MSG msg) {
    return 0;
}

long XBaseWindow::OnClose(MSG msg) {
    XDestroyWindow(m_x11Window->display,m_x11Window->window);
    m_x11Window->window = 0;
    if(msg.xclient.message_type == XInternAtom(m_x11Window->display, "UI_WINDOW_CLOSE_RESPONSE", False)){
        modalRetValue = msg.xclient.data.l[0];
    }else{
        modalRetValue = UI_DLG_BUTTON_CLOSE;
    }
    return 0;
}

long XBaseWindow::OnMouseMove(MSG msg) {
    return 0;
}

long XBaseWindow::OnMouseWheel(MSG msg) {
    return 0;
}

void XBaseWindow::MessageHandler(MSG event) {
    Atom wm_protocols = XInternAtom(m_x11Window->display, "WM_PROTOCOLS", False);
    Atom wm_delete_window = XInternAtom(m_x11Window->display, "WM_DELETE_WINDOW", False);
    switch (event.type) {
        case DestroyNotify:
            this->OnFinalMessage();
            glbContinueRunning = false;
            break;
        case Expose:
            if(event.xexpose.count==0){
                this->OnPaint(event);
            }
            break;
        case KeyPress:
            this->OnKeyPress(event);
            break;
        case KeyRelease:
            this->OnKeyRelease(event);
            break;
        case ButtonPress:
            if(event.xbutton.button == Button1){
                this->OnLeftButtonPress(event);
            }else if(event.xbutton.button == Button3){
                this->OnRightButtonPress(event);
            }else if(event.xbutton.button == Button4 || event.xbutton.button==Button5){
                this->OnMouseWheel(event);
            }
            break;
        case ButtonRelease:
            if(event.xbutton.button == Button1){
                this->OnLeftButtonRelease(event);
            }else if(event.xbutton.button == Button3){
                this->OnRightButtonRelease(event);
            }
            break;
        case MotionNotify:
            this->OnMouseMove(event);
            break;
        case ConfigureNotify:
            this->OnReSize(event);
            break;
        case ClientMessage:
            if (event.xclient.message_type == wm_protocols &&
                event.xclient.data.l[0] == wm_delete_window) {
                this->OnClose(event);
            }else if (event.xclient.message_type == XInternAtom(m_x11Window->display, "UI_WINDOW_CLOSE_RESPONSE", False)) {
                //event.xclient.data.l[0];
                this->OnClose(event);
            }
            break;
        default:
            break;
    }
}

void XBaseWindow::Close(UIDlgButtonId id) {
    Atom my_dialog_response = XInternAtom(m_x11Window->display, "UI_WINDOW_CLOSE_RESPONSE", False);

    XEvent event;
    event.type = ClientMessage;
    event.xclient.window = m_x11Window->window;
    event.xclient.message_type = my_dialog_response;
    event.xclient.format = 32;
    event.xclient.data.l[0] = id;

    XSendEvent(m_x11Window->display, m_x11Window->window, False, NoEventMask, &event);
    XFlush(m_x11Window->display);
}
