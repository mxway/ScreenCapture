#ifndef ICONTHEME_ICONTHEME_H
#define ICONTHEME_ICONTHEME_H
#include <X11/Xlib.h>
#include <string>
#include <memory>
#include "inicpp.h"

using namespace std;

class IconTheme {
public:
    explicit IconTheme(const string &iconThemeName);
    virtual ~IconTheme();
    virtual string GetIconType(const string &fileName);
    virtual string GetIconFileName(const string &mimeType, uint32_t iconSize);
    virtual IconTheme *Copy(const string &parentThemeName) = 0;
    void    PushParentTheme(IconTheme *parent);
    string GetIconThemeName()const{
        return m_iconThemeName;
    }

    string GetIconFileFromCurrentTheme(const string &mimeType,uint32_t iconSize);

    string GetIconFileFromParents(const string &mimeType, uint32_t iconSize);

protected:
    string GetIconFileNameFromIndexTheme(const string &expectContext, const string &expectType, const string &mimeType, uint32_t iconSize= -1);
protected:
    virtual string GetFixedIcon(const string &possibleMimeType, size_t iconSize=-1) = 0;
    virtual string GetScalableIcon(const string &possibleMimeType) = 0;
    virtual vector<string>  GetPossibleMimeTypes(const string &detectedMimeType) = 0;

protected:
    ini::IniFile    m_iniFile;
    string          m_iconThemeName;
    string          m_iconFolder;
    vector<IconTheme*> m_parents;
};

class IconThemeFactory{
public:
    static shared_ptr<IconTheme> GetInstanceByFileName(const string &fileName);
};


#endif //ICONTHEME_ICONTHEME_H
