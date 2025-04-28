#ifndef FILEDIALOG_X11WINDOW_H
#define FILEDIALOG_X11WINDOW_H
#include <X11/Xlib.h>

typedef struct _X11Window_s{
    Display     *display;
    int         screen;
    Window      window;
    Visual      *visual;
    Colormap    colormap;
    Pixmap      offscreenPixmap;
    int         depth;
    int         x;
    int         y;
    int         width;
    int         height;
    GC          gc;
}X11Window;

typedef XEvent MSG;

#endif //FILEDIALOG_X11WINDOW_H
