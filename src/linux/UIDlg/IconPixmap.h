#ifndef FILEDIALOG_ICONPIXMAP_H
#define FILEDIALOG_ICONPIXMAP_H
#include <X11/Xlib.h>

Pixmap CreateIconPixmap(Display *display, Window window,int width, int height, const char *fileName);

#endif //FILEDIALOG_ICONPIXMAP_H
