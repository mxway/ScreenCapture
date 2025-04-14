#include "WindowBitmap.h"

static unsigned char * GetBmpFromDc(HDC hdc,HBITMAP bitMapHandle){
    BITMAPINFO	bmi;
    unsigned int lineSize = 0;
    BITMAP	bitmap;
    if(!::GetObject(bitMapHandle,sizeof(BITMAP),&bitmap))
    {
        return nullptr;
    }
    bitmap.bmBitsPixel = 24;
    memset(&bmi,0,sizeof(BITMAPINFO));
    bmi.bmiHeader.biBitCount = bitmap.bmBitsPixel;
    bmi.bmiHeader.biCompression = 0;
    bmi.bmiHeader.biWidth = bitmap.bmWidth;
    bmi.bmiHeader.biHeight = bitmap.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    lineSize = bmi.bmiHeader.biWidth*3;
    lineSize = (lineSize+3)&~(3);
    unsigned totalSize = lineSize * 3 * bmi.bmiHeader.biHeight;
    unsigned char *result = (unsigned char*)malloc(bitmap.bmWidth * bitmap.bmHeight * 3);
    // GetDIBits,pixels for each row of picture should be the width align with 4.but when we use stbi_write_png/stbi_write_jpg/stbi_write_bmp,
    //we just needs (width) ,the padding pixel(s) should be ignore.
    unsigned char *tmpBuffer = (unsigned char*)malloc(totalSize);
    if(!::GetDIBits(hdc,bitMapHandle,0,bitmap.bmHeight,tmpBuffer,&bmi,DIB_RGB_COLORS))
    {
        return nullptr;;
    }

    //tmpBuffer's data is stored in reverse order of row.
    //so first row of data stored in tmpBuffer,should be the last row of data of the actual picture

    //should flip the picture data by each row , each pixel use 3bytes(red,green,blue)
    //bytes of each line should the number of pixels times 3
    for (int i=0;i<bitmap.bmHeight;i++) {
        unsigned char *originBytes = tmpBuffer + (bitmap.bmHeight-i-1)*lineSize;
        unsigned char *dst = result + i*bitmap.bmWidth*3;
        for (int j=0;j<bitmap.bmWidth;j++) {
            dst[j*3] = originBytes[j*3+2];
            dst[j*3+1] = originBytes[j*3+1];
            dst[j*3+2] = originBytes[j*3];
        }
    }
    free(tmpBuffer);
    return result;
}

ImageBitmapInfo GetImageBitmapInfoFromWindow(HWND hwnd, HDC srcHDC,RECT rect){
    ImageBitmapInfo bmpInfo {0};
    bmpInfo.width = rect.right - rect.left + 1;
    bmpInfo.height = rect.bottom - rect.top + 1;
    HDC hdc = ::GetDC(hwnd);
    HDC	selfDc = ::CreateCompatibleDC(hdc);
    HBITMAP hbitMap = ::CreateCompatibleBitmap(hdc,bmpInfo.width,bmpInfo.height);
    HGDIOBJ oldObject = ::SelectObject(selfDc,hbitMap);
    ::BitBlt(selfDc,0,0,bmpInfo.width,bmpInfo.height,
        srcHDC,rect.left,rect.top,SRCCOPY);
    bmpInfo.data = GetBmpFromDc(selfDc,hbitMap);
    ::ReleaseDC(hwnd,hdc);
    ::DeleteDC(selfDc);
    ::DeleteObject(hbitMap);
    return bmpInfo;
}