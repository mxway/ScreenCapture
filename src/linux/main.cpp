#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrender.h>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "../UIRect.h"
#include "WindowBitmap.h"
#include "../ImageFactory.h"
#include "UIDlg/FileDialog.h"
#include "UIDlg/Misc.h"

using namespace std;

Pixmap rootPixmap;
Pixmap darkPixmap;

int glbLeftMousePressed = 0;
int mouseX = 0;
int mouseY = 0;

void setUtf8WindowTitle(Display *display, Window window, const char *title) {
    Atom net_wm_name = XInternAtom(display, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);

    XChangeProperty(display, window, net_wm_name, utf8_string, 8,
                    PropModeReplace, (unsigned char *)title, (int)strlen(title));
}

void CopyBitmapFromRootWindow(Display *display, Window window,int width, int height)
{
    int screen = DefaultScreen(display);
    if(rootPixmap != 0){
        XFreePixmap(display,rootPixmap);
        rootPixmap = 0;
    }
    rootPixmap = XCreatePixmap(display,window,width,height, DefaultDepth(display,screen));
    darkPixmap = XCreatePixmap(display,window, width,height, DefaultDepth(display,screen));
    Pixmap blackBackground = XCreatePixmap(display, window,width,height, 32);
    GC gc = XCreateGC(display,rootPixmap,0, nullptr);
    XImage *rootImage = XGetImage(display, RootWindow(display,screen),0,0,width,height,AllPlanes,ZPixmap);
    
    XPutImage(display,rootPixmap, gc,rootImage,0,0,0,0,width,height);
    XDestroyImage(rootImage);

    XRenderPictFormat *format {nullptr};
    if(DefaultDepth(display,screen) == 32){
        format = XRenderFindStandardFormat(display, PictStandardARGB32);
    }else{
        format = XRenderFindStandardFormat(display, PictStandardRGB24);
    }

    XRenderColor color = {0x0,0x0,0x0,0x8000};
    Picture blackBackgroundPicture = XRenderCreatePicture(display,blackBackground,XRenderFindStandardFormat(display, PictStandardARGB32),0,nullptr);
    XRenderFillRectangle(display,PictOpSrc,blackBackgroundPicture,&color,0,0,width,height);

    Picture picture = XRenderCreatePicture(display,rootPixmap, format,0, nullptr);
    Picture window_picture = XRenderCreatePicture(display,darkPixmap,format,0,nullptr);

    XRenderComposite(display,PictOpOver,picture, None,window_picture,0,0,0,0,0,0,width,height);
    XRenderComposite(display,PictOpOver,blackBackgroundPicture, None,window_picture,0,0,0,0,0,0,width,height);

    XRenderFreePicture(display,blackBackgroundPicture);
    XRenderFreePicture(display,window_picture);
    XRenderFreePicture(display, picture);
    XFreeGC(display,gc);

    XFreePixmap(display,blackBackground);
}

int main() {
    Display *display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        fprintf(stderr, "Can't connect to X Server\n");
        return 1;
    }

    int screen = DefaultScreen(display);

    int width = DisplayWidth(display,screen);
    int height = DisplayHeight(display,screen);

    Cursor crossCursor = XCreateFontCursor(display,XC_cross);
    CopyBitmapFromRootWindow(display, RootWindow(display,screen),width, height);

    Window window = XCreateSimpleWindow(
            display, RootWindow(display, screen),
            0, 0, width, height, 1,
            BlackPixel(display, screen),
            WhitePixel(display, screen));

    Atom wm_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom wm_type_dock = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
    XChangeProperty(display, window, wm_type, XA_ATOM, 32, PropModeReplace,
                    (unsigned char *)&wm_type_dock, 1);

    // Disable window decorate,we will create a window without title bar and border
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom wm_state_skip_taskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    Atom wm_state_skip_pager = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);
    XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace,
                    (unsigned char *)&wm_state_skip_taskbar, 1);
    XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeAppend,
                    (unsigned char *)&wm_state_skip_pager, 1);

    // setting the window to top level
    Atom wm_state_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);
    XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeAppend,
                    (unsigned char *)&wm_state_above, 1);

    XDefineCursor(display,window,crossCursor);
    // setting override_redirect property，so we can capture the full screen.
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    XChangeWindowAttributes(display, window, CWOverrideRedirect, &attrs);

    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);

    setUtf8WindowTitle(display,window,u8"Screen Capture");

    GC gc = XCreateGC(display,window,0,nullptr);

    XMapWindow(display, window);

    UIRect oldRect = {-1,-1,0,0};

    XEvent event;
    bool continueRunning = true;
    while (continueRunning) {
        XNextEvent(display, &event);
        if(event.type == Expose && event.xexpose.count ==  0){
            XSetInputFocus(display,window,RevertToParent, CurrentTime);
            XCopyArea(display,darkPixmap,window,gc,0,0,width,height,0,0);
        }else if(event.type == ConfigureNotify){

        }else if(event.type == ButtonPress) {
            if(event.xbutton.button == Button3){
                continueRunning = false;
            }else if(event.xbutton.button == Button1){
                glbLeftMousePressed = 1;
                mouseX = event.xbutton.x;
                mouseY = event.xbutton.y;
                XCopyArea(display,darkPixmap,window,gc,0,0,width,height,0,0);
            }
        }else if(event.type == ButtonRelease && (event.xbutton.state & Button1Mask) && glbLeftMousePressed) {
            glbLeftMousePressed = 0;
            FileDialog  fileDialog;
            fileDialog.Create(100,100,800,600,"Save File",window,WindowType_Popup);
            uint32_t ret = fileDialog.Exec();
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
                ImageBitmapInfo imageBitmapInfo = GetImageBitmapInfoFromWindow(display,rootPixmap,oldRect);
                // GetImage
                // 1 -- save picture as  bmp file format
                // 2 -- save picture as png file format
                // 3 -- save picture as jpg file format
                CImage *image = CImageFactory::GetImage(imageType);
                image->saveImage(savedFile,imageBitmapInfo);
                free(imageBitmapInfo.data);
                delete image;
                continueRunning = false;
            }
            continueRunning = false;
            oldRect.x = oldRect.y = -1;
            oldRect.width = oldRect.height = 0;
        }else if(event.type == MotionNotify) {
            if( (event.xmotion.state & Button1Mask) && glbLeftMousePressed){
                int startX = mouseX<event.xbutton.x?mouseX:event.xbutton.x;
                int startY = mouseY<event.xbutton.y?mouseY:event.xbutton.y;
                int tmpWidth = abs(mouseX-event.xbutton.x);
                int tmpHeight = abs(mouseY-event.xbutton.y);

                UIRect newRect = {startX,startY,tmpWidth,tmpHeight};

                XSetForeground(display,gc, 0xFFFF0000);
                char dash_list[] = {10,20};
                int dash_offset = 0;
                XSetDashes(display,gc,dash_offset,dash_list,2);

                XSetLineAttributes(display,gc,1,LineOnOffDash,CapButt,JoinMiter);

                UIRect diffRectArray[4] = {0};
                int count = 0;
                rect_difference(&newRect, &oldRect,diffRectArray,&count);
                for(auto & diffRect : diffRectArray){
                    if(diffRect.width <= 0 || diffRect.height <= 0){
                        continue;
                    }
                    XCopyArea(display, rootPixmap, window, gc, diffRect.x, diffRect.y,
                              diffRect.width + 1, diffRect.height + 1, diffRect.x, diffRect.y);
                }
                memset(diffRectArray,0,sizeof(diffRectArray));
                rect_difference(&oldRect,&newRect,diffRectArray,&count);
                for(auto & rangeRect : diffRectArray){
                    if(rangeRect.width <= 0 || rangeRect.height <= 0){
                        continue;
                    }
                    XCopyArea(display, darkPixmap, window, gc, rangeRect.x, rangeRect.y,
                              rangeRect.width + 1, rangeRect.height + 1, rangeRect.x, rangeRect.y);
                }
                oldRect = newRect;
                XCopyArea(display,rootPixmap,window,gc,oldRect.x,oldRect.y,1,oldRect.height,oldRect.x,oldRect.y);
                XCopyArea(display,rootPixmap,window,gc,oldRect.x,oldRect.y,oldRect.width,1,oldRect.x,oldRect.y);
                XCopyArea(display,rootPixmap,window,gc, oldRect.x, oldRect.y+oldRect.height,oldRect.width,1,oldRect.x,oldRect.y+oldRect.height);
                XCopyArea(display,rootPixmap,window,gc, oldRect.x+oldRect.width,oldRect.y,1,oldRect.height,oldRect.x+oldRect.width,oldRect.y);
                XDrawRectangle(display,window,gc, oldRect.x,oldRect.y,oldRect.width,oldRect.height);
            }
        }else if(event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey,0);
            if(key == XK_Escape){
                continueRunning = false;
            }
        }
    }
    XFreePixmap(display,darkPixmap);
    XFreePixmap(display, rootPixmap);
    XFreeCursor(display,crossCursor);
    XFreeGC(display,gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}