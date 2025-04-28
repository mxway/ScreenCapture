#ifndef FILEDIALOG_PIXMAPCACHE_H
#define FILEDIALOG_PIXMAPCACHE_H
#include <map>
#include <string>
#include <X11/Xlib.h>
#include "X11Window.h"

using namespace std;

class PixmapCache {
public:
    static PixmapCache *GetInstance();
    void    Clear(X11Window *window);
    void    AddMimeTypePixmapPair(const string &mimeType,Pixmap pixmap);
    Pixmap  GetPixmapByMimeType(const string &mimeType);
private:
    map<string,Pixmap> m_mimeTypePixmapMapping;
};


#endif //FILEDIALOG_PIXMAPCACHE_H
