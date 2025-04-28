#include "PixmapCache.h"

PixmapCache *PixmapCache::GetInstance() {
    static PixmapCache pixmapCache;
    return &pixmapCache;
}

void PixmapCache::Clear(X11Window *window) {
    for(auto &itr : m_mimeTypePixmapMapping){
        XFreePixmap(window->display,itr.second);
    }
    m_mimeTypePixmapMapping.clear();
}

void PixmapCache::AddMimeTypePixmapPair(const string &mimeType, Pixmap pixmap) {
    m_mimeTypePixmapMapping.insert(make_pair(mimeType,pixmap));
}

Pixmap PixmapCache::GetPixmapByMimeType(const string &mimeType) {
    auto itr = m_mimeTypePixmapMapping.find(mimeType);
    if(itr != m_mimeTypePixmapMapping.end()){
        return itr->second;
    }
    return 0;
}
