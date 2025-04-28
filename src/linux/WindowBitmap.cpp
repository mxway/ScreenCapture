#include "WindowBitmap.h"
ImageBitmapInfo GetImageBitmapInfoFromWindow(Display *display, Drawable drawable,UIRect rect){
    ImageBitmapInfo    imageBitmapInfo{0};
    XImage *image = XGetImage(display, drawable,rect.x, rect.y,rect.width,rect.height,AllPlanes,ZPixmap);
    imageBitmapInfo.width = rect.width;
    imageBitmapInfo.height = rect.height;
    imageBitmapInfo.data = malloc(rect.width*rect.height*3);
    auto *rowData = static_cast<unsigned char *>(imageBitmapInfo.data);
    for(int y=0;y<rect.height;y++){
        rowData = (unsigned char*)imageBitmapInfo.data + y * rect.width*3;
        for(int x=0;x<rect.width;x++){
            unsigned long pixel = XGetPixel(image,x,y);
            rowData[3*x+0] = (pixel>>16)&0xFF;
            rowData[3*x+1] = (pixel>>8)&0xFF;
            rowData[3*x+2] = (pixel>>0)&0xFF;
        }
    }

    return imageBitmapInfo;
}