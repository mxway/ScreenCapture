#include "XBaseWindow.h"
#include <cstring>
#include <cstdio>
#include <X11/keysym.h>
#include <locale.h>

static Display *glbDisplay = nullptr;

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
    if(m_x11Window->display!= nullptr){
        XCloseDisplay(m_x11Window->display);
    }
    delete m_x11Window;
}

bool XBaseWindow::Create(int x, int y, int width, int height, const char *title, Window parent,WindowType windowType) {
    setlocale(LC_ALL, "");
    if(m_x11Window->display == nullptr){
        glbDisplay = XOpenDisplay(nullptr);
        if(glbDisplay == nullptr){
            fprintf(stderr,"Can't connect to X Server...\n");
            return false;
        }
    }
    m_x11Window->display = glbDisplay;
    m_x11Window->screen = DefaultScreen(glbDisplay);
    m_x11Window->depth = DefaultDepth(glbDisplay,m_x11Window->screen);
    m_x11Window->visual = DefaultVisual(glbDisplay,m_x11Window->screen);
    m_x11Window->colormap = DefaultColormap(glbDisplay,m_x11Window->screen);
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

void XBaseWindow::SetTitle(const char *title) {
    Atom net_wm_name = XInternAtom(m_x11Window->display, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(m_x11Window->display, "UTF8_STRING", False);

    XChangeProperty(m_x11Window->display, m_x11Window->window, net_wm_name, utf8_string, 8,
                    PropModeReplace, (unsigned char *)title, strlen(title));
}

static bool glbContinueRunning = true;

void XBaseWindow::MessageLoop() {
    XEvent event;

    Atom wm_protocols = XInternAtom(m_x11Window->display, "WM_PROTOCOLS", False);
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
    KeySym keysym = XLookupKeysym(&msg.xkey, 0);

    int modifiers = msg.xkey.state;
    if ((modifiers & ControlMask) && keysym == XK_c) {
        printf("Ctrl + C\n");
        this->Close(UI_DLG_BUTTON_CLOSE);
        //this->OnClose(msg);
        glbContinueRunning = false;
    }
    return 0;
}

long XBaseWindow::OnKeyRelease(MSG msg) {
    return 0;
}

long XBaseWindow::OnClose(MSG msg) {
    XDestroyWindow(m_x11Window->display,m_x11Window->window);
    m_x11Window->window = 0;
    //glbContinueRunning = false;
    return 0;
}

long XBaseWindow::OnMouseMove(MSG msg) {
    return 0;
}

long XBaseWindow::OnMouseWheel(MSG msg) {
    return 0;
}

void XBaseWindow::MessageHandler(MSG event) {
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
        default:
            break;
    }
}

void XBaseWindow::Close(UIDlgButtonId id) {
    Atom my_dialog_response = XInternAtom(m_x11Window->display, "DIALOG_RESPONSE", False);

    XEvent event;
    event.type = ClientMessage;
    event.xclient.window = m_x11Window->window;
    event.xclient.message_type = my_dialog_response;
    event.xclient.format = 32;
    event.xclient.data.l[0] = id;

    XSendEvent(m_x11Window->display, m_x11Window->window, False, NoEventMask, &event);
    XFlush(m_x11Window->display);
}
