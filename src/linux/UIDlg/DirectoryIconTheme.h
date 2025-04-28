#ifndef FILEDIALOG_DIRECTORYICONTHEME_H
#define FILEDIALOG_DIRECTORYICONTHEME_H
#include "IconTheme.h"

class DirectoryIconTheme : public IconTheme {
public:
    explicit DirectoryIconTheme(const string &iconTheme);

    string GetIconType(const string &fileName) override;

    string GetIconFileName(const string &mimeType, uint32_t iconSize) override;

    IconTheme *Copy(const string &parentThemeName) override;

protected:
    string GetFixedIcon(const string &possibleMimeType, size_t iconSize) override;

    string GetScalableIcon(const string &possibleMimeType) override;

    vector<string> GetPossibleMimeTypes(const string &detectedMimeType) override;

    string GetUserSpecialFolderIcon(const string &mimeType,size_t iconSize);
};


#endif //FILEDIALOG_DIRECTORYICONTHEME_H
