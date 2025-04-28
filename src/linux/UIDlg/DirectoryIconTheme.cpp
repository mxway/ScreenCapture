#include "DirectoryIconTheme.h"
#include <set>
#include "UserDirs.h"

DirectoryIconTheme::DirectoryIconTheme(const string &iconTheme)
    : IconTheme(iconTheme)
{

}

string DirectoryIconTheme::GetFixedIcon(const string &possibleMimeType, size_t iconSize) {
    return this->GetIconFileNameFromIndexTheme("Places", "Fixed", possibleMimeType, iconSize);
}

string DirectoryIconTheme::GetScalableIcon(const string &possibleMimeType) {
    return this->GetIconFileNameFromIndexTheme("Places", "Scalable", possibleMimeType);
}

vector<string> DirectoryIconTheme::GetPossibleMimeTypes(const string &detectedMimeType) {
    vector<string> possibleMimeTypes = {"inode/directory", "folder"};
    possibleMimeTypes.push_back(detectedMimeType);
    return possibleMimeTypes;
}

IconTheme *DirectoryIconTheme::Copy(const string &parentThemeName) {
    return new DirectoryIconTheme(parentThemeName);
}

string DirectoryIconTheme::GetIconFileName(const string &mimeType, uint32_t iconSize) {
    //map<string,string> userSpecialFoldersMapping;
    static set<string>  userSpecialFolders = {
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Desktop),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Documents),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Download),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Music),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Pictures),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_PublicShare),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Templates),
            UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Videos)
    };
    string iconFileName = this->GetUserSpecialFolderIcon(mimeType,iconSize);
    if(iconFileName.length()!=0){
        return iconFileName;
    }
    return IconTheme::GetIconFileName(mimeType, iconSize);
}

string DirectoryIconTheme::GetUserSpecialFolderIcon(const string &mimeType,size_t iconSize) {
    static map<string,vector<string>> specialFolderMimeTypeMapping = {
            {"inode-folder-desktop",{"folder-desktop","folder_desktop","user-desktop","user_desktop","desktop"}},
            {"inode-folder-documents",{"folder-documents","folder_documents","user-documents","documents"}},
            {"inode-folder-download",{"folder-download","folder-downloads","folder_download","folder_downloads","user-download","download"}},
            {"inode-folder-music",{"folder-music","folder_music","user-music","user_music","music"}},
            {"inode-folder-pictures",{"folder-pictures","folder_pictures","user-pictures","user_pictures","pictures"}},
            {"inode-folder-publicshare",{"folder-publicshare","folder_publicshare","user-publicshare","user_publicshare","publicshare"}},
            {"inode-folder-templates",{"folder-templates","folder_templates","user-templates","user_templates","templates"}},
            {"inode-folder-video",{"folder-video","folder-videos","folder_video","folder_videos","user-video","user-videos","user_video","user_videos","video"}}
    };
    auto itr = specialFolderMimeTypeMapping.find(mimeType);
    if(itr == specialFolderMimeTypeMapping.end()){
        return "";
    }
    for(auto &mimeType:itr->second){
        string fileName = this->GetFixedIcon(mimeType,iconSize);
        if(!fileName.empty()){
            return fileName;
        }
        fileName = this->GetScalableIcon(mimeType);
        if(!fileName.empty()){
            return fileName;
        }
    }

    for(auto &possibleMimeType:itr->second){
        for(auto &parent: m_parents){
            string fileName = parent->GetIconFileFromCurrentTheme(possibleMimeType,iconSize);
            if(!fileName.empty()){
                return fileName;
            }
        }
    }

    //now,we haven't got icon file from the current theme and it's all parents.
    //search for it's grandparents
    for(auto &possibleMimeType : itr->second){
        for(auto &parent : m_parents){
            string fileName = parent->GetIconFileFromParents(possibleMimeType,iconSize);
            if(!fileName.empty()){
                return fileName;
            }
        }
    }
    return std::string();
}

string DirectoryIconTheme::GetIconType(const string &fileName) {
    static map<string,string> userSpecialFolderMimeTypeMapping = {
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Desktop),"inode-folder-desktop"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Documents),"inode-folder-documents"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Download),"inode-folder-download"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Music),"inode-folder-music"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Pictures),"inode-folder-pictures"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_PublicShare),"inode-folder-publicshare"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Templates),"inode-folder-templates"},
            {UserDirs::GetInstance().GetUserSpecialDir(SpecialUserDir_Videos),"inode-folder-video"}
    };
    auto itr = userSpecialFolderMimeTypeMapping.find(fileName);
    if(itr != userSpecialFolderMimeTypeMapping.end()){
        return itr->second;
    }
    return "inode-directory";
}
