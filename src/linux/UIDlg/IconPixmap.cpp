#include "IconPixmap.h"
#include <list>
#include <lunasvg.h>
#include <X11/Xutil.h>
#include "IconTheme.h"
#define STB_IMAGE_IMPLEMENTATION 1
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION 1
#define STBIR__HEADER_FILENAME
#include <stb_image_resize2.h>
#include "Misc.h"
#include "PixmapCache.h"

using namespace std;
using namespace lunasvg;

static unsigned char *GetSvgData(const char *fileName, int width,int height)
{
    auto document = Document::loadFromFile(fileName);
    auto bitmap = document->renderToBitmap(width,height);
    unsigned char *data = static_cast<unsigned char *>(malloc(width * height * 4));
    memcpy(data, bitmap.data(),width*height*4);
    return data;
}

static unsigned char *GetPngData(const char *fileName,int expectWidth,int expectHeight) {
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc *data = stbi_load(fileName, &width, &height, &channels, 4);
    unsigned char *rgbData = (unsigned char *) malloc(width * height * 4);
    for (int i = 0; i < width * height; i++) {
        rgbData[i * 4 + 3] = data[i * 4 + 3];
        if (rgbData[i * 4 + 3] < 255) {
            rgbData[i * 4] = (uint8_t) (uint32_t(data[i * 4 + 2]) * data[i * 4 + 3] / 255);
            rgbData[i * 4 + 1] = (uint8_t) (uint32_t(data[i * 4 + 1]) * data[i * 4 + 3] / 255);
            rgbData[i * 4 + 2] = (uint8_t) (uint32_t(data[i * 4]) * data[i * 4 + 3] / 255);
        } else {
            rgbData[i * 4] = data[i * 4 + 2];
            rgbData[i * 4 + 1] = data[i * 4 + 1];
            rgbData[i * 4 + 2] = data[i * 4];
        }
    }
    stbi_image_free(data);
    unsigned char *result = (unsigned char *) malloc(int(expectWidth * expectHeight * 4));
    stbir_resize(rgbData, width, height, 0, result, expectWidth, expectHeight,
                 0, STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP,
                 STBIR_FILTER_POINT_SAMPLE);
    free(rgbData);
    return result;
}

//static map<string,Pixmap> pixmapCache;

Pixmap CreateIconPixmap(Display *display, Window window,int width, int height, const char *fileName) {
    shared_ptr<IconTheme> iconTheme = IconThemeFactory::GetInstanceByFileName(fileName);
    string mimeType = iconTheme->GetIconType(fileName);
    Pixmap cachePixmap = PixmapCache::GetInstance()->GetPixmapByMimeType(mimeType);
    if(cachePixmap != 0){
        return cachePixmap;
    }
    string iconFileName = iconTheme->GetIconFileName(mimeType, width);
    Pixmap pixmap = XCreatePixmap(display,window,width,height,32);
    GC pixmapGC = XCreateGC(display, pixmap, 0, NULL);
    unsigned char *data = nullptr;
    if(EndWith(iconFileName,".svg")){
        data = GetSvgData(iconFileName.c_str(),width,height);
    }else{
        //png,jpeg,bmp?
        data = GetPngData(iconFileName.c_str(),width,height);
    }

    XImage *ximage = XCreateImage(display, DefaultVisual(display, DefaultScreen(display)), 32, ZPixmap, 0,
                                  reinterpret_cast<char *>(data),
                                  width, height, 32, 0);
    XPutImage(display, pixmap, pixmapGC, ximage, 0, 0, 0, 0, width, height);
    PixmapCache::GetInstance()->AddMimeTypePixmapPair(mimeType,pixmap);
    XFreeGC(display, pixmapGC);
    XDestroyImage(ximage);
    return pixmap;
}
