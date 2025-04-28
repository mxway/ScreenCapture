#include "FileIconTheme.h"

FileIconTheme::FileIconTheme(const string &themeName)
    : IconTheme(themeName)
{

}

string FileIconTheme::GetFixedIcon(const string &possibleMimeType, size_t iconSize) {
    return this->GetIconFileNameFromIndexTheme("MimeTypes","Fixed",possibleMimeType,iconSize);
}

string FileIconTheme::GetScalableIcon(const string &possibleMimeType) {
    return this->GetIconFileNameFromIndexTheme("MimeTypes", "Scalable", possibleMimeType);
}

vector<string> FileIconTheme::GetPossibleMimeTypes(const string &detectedMimeType) {
    vector<string> possibleMimeTypes;
    possibleMimeTypes.push_back(detectedMimeType);
    int pos = detectedMimeType.find("/");
    if(pos!=string::npos){
        possibleMimeTypes.push_back(detectedMimeType.substr(0,pos) + "/x-generic");
    }
    possibleMimeTypes.push_back("text-x-generic");
    possibleMimeTypes.push_back("text-plain");
    possibleMimeTypes.push_back("text-x-plain");
    return possibleMimeTypes;
}

IconTheme *FileIconTheme::Copy(const string &parentThemeName) {
    return new FileIconTheme(parentThemeName);
}
