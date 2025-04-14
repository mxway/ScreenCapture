#ifndef WINDOWBITMAP_H
#define WINDOWBITMAP_H
#include <windows.h>
#include "../ImageBitmapInfo.h"

ImageBitmapInfo GetImageBitmapInfoFromWindow(HWND hwnd, HDC srcDC,RECT rect);

#endif //WINDOWBITMAP_H
