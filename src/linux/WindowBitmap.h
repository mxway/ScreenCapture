#ifndef SCREENCAPTURE_WINDOWBITMAP_H
#define SCREENCAPTURE_WINDOWBITMAP_H
#include "../ImageBitmapInfo.h"
#include "../UIRect.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

ImageBitmapInfo GetImageBitmapInfoFromWindow(Display *display, Drawable drawable,UIRect rect);


#endif //SCREENCAPTURE_WINDOWBITMAP_H
