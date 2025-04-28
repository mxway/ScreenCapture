#ifndef FILEDIALOG_FILEICONTHEME_H
#define FILEDIALOG_FILEICONTHEME_H
#include "IconTheme.h"

class FileIconTheme : public IconTheme{
public:
    explicit FileIconTheme(const string &themeName);

    IconTheme *Copy(const string &parentThemeName) override;

protected:
    string GetFixedIcon(const string &possibleMimeType, size_t iconSize) override;

    string GetScalableIcon(const string &possibleMimeType) override;

    vector<string> GetPossibleMimeTypes(const string &detectedMimeType) override;
};


#endif //FILEDIALOG_FILEICONTHEME_H
