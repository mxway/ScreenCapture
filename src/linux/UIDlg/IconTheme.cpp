#include "IconTheme.h"
#include "inicpp.h"
#include "Misc.h"
#include <list>
#include <cstring>
#include <libgen.h>

using namespace std;

static string GetPictureName(const string &mimeType){
    string result;
    for(auto &c : mimeType){
        if(c == '/'){
            result.append(1,'-');
        }else{
            result.append(1,c);
        }
    }
    return result;
}

static string GetIconFileName(const string &iconThemeDirectory,const string &subDirectory,const string &mimeType){
    string fullPictureNameWithoutExt = iconThemeDirectory + "/" + subDirectory + "/" + GetPictureName(mimeType);
    if(IsFileExists(fullPictureNameWithoutExt+".png")){
        return fullPictureNameWithoutExt + ".png";
    }else if(IsFileExists(fullPictureNameWithoutExt + ".svg")){
        return fullPictureNameWithoutExt + ".svg";
    }
    return "";
}

class FileNameMimeTypeCache
{
public:
    ~FileNameMimeTypeCache(){
        m_fileNameMimeTypeMapping.clear();
    }
    static FileNameMimeTypeCache *GetInstance(){
        static FileNameMimeTypeCache fileNameMimeTypeCache;
        return &fileNameMimeTypeCache;
    }
    string  GetFileNameMimeType(const string &fileName){
        if(m_fileNameMimeTypeMapping.size()>0){
            string mappingDir = GetFilePath(m_fileNameMimeTypeMapping.begin()->first);
            string currentDir = GetFilePath(fileName);
            if(mappingDir != currentDir){
                m_fileNameMimeTypeMapping.clear();
            }
        }
        auto itr = m_fileNameMimeTypeMapping.find(fileName);
        if(itr != m_fileNameMimeTypeMapping.end()){
            return itr->second;
        }
        return "";
    }
    void  AddFilenameMimeType(const string &fileName, const string &mimeType){
        m_fileNameMimeTypeMapping.insert(make_pair(fileName,mimeType));
    }
private:
    static string GetFilePath(const string &fileName){
        char *tmpBuf = new char[fileName.length()+1];
        memset(tmpBuf,0,fileName.length()+1);
        memcpy(tmpBuf,fileName.c_str(),fileName.length());
        string result = dirname(tmpBuf);
        delete []tmpBuf;
        return result;
    }
private:
    map<string,string>      m_fileNameMimeTypeMapping;
};

IconTheme::IconTheme(const string &iconThemeName)
    : m_iconThemeName{iconThemeName}
{
    m_iconFolder = string("/usr/share/icons/") + m_iconThemeName;
    string iconThemeIndexTheme = string("/usr/share/icons/") + m_iconThemeName + "/index.theme";
    m_iniFile.load(iconThemeIndexTheme);
}

IconTheme::~IconTheme() {
    for(auto &iconTheme : m_parents){
        delete iconTheme;
    }
}

string IconTheme::GetIconType(const string &fileName) {
    string mimeType = FileNameMimeTypeCache::GetInstance()->GetFileNameMimeType(fileName);
    if(!mimeType.empty()){
        return mimeType;
    }
    mimeType = GetFileContentType(fileName);
    FileNameMimeTypeCache::GetInstance()->AddFilenameMimeType(fileName,mimeType);
    return mimeType;
}

string IconTheme::GetIconFileName(const string &mimeType, uint32_t iconSize) {
    string iconFileName = this->GetIconFileFromCurrentTheme(mimeType, iconSize);
    if(iconFileName.empty()){
        iconFileName = this->GetIconFileFromParents(mimeType, iconSize);
    }
    return iconFileName;
}

string IconTheme::GetIconFileFromCurrentTheme(const string &mimeType, uint32_t iconSize) {
    vector<string> possibleMimeTypes = this->GetPossibleMimeTypes(mimeType);
    for(auto &possibleMimeType : possibleMimeTypes){
        string fileName = this->GetFixedIcon(possibleMimeType,iconSize);
        if(!fileName.empty()){
            return fileName;
        }
        fileName = this->GetScalableIcon(possibleMimeType);
        if(!fileName.empty()){
            return fileName;
        }
    }
    return "";
}

string IconTheme::GetIconFileFromParents(const string &mimeType, uint32_t iconSize) {
    vector<string> possibleMimeTypes = this->GetPossibleMimeTypes(mimeType);
    for(auto &possibleMimeType:possibleMimeTypes){
        for(auto &parent: m_parents){
            string fileName = parent->GetIconFileFromCurrentTheme(possibleMimeType,iconSize);
            if(!fileName.empty()){
                return fileName;
            }
        }
    }

    //now,we haven't got icon file from the current theme and it's all parents.
    //search for it's grandparents
    for(auto &possibleMimeType : possibleMimeTypes){
        for(auto &parent : m_parents){
            string fileName = parent->GetIconFileFromParents(possibleMimeType,iconSize);
            if(!fileName.empty()){
                return fileName;
            }
        }
    }

    return "";
}

void IconTheme::PushParentTheme(IconTheme *parent) {
    m_parents.push_back(parent);
}

string
IconTheme::GetIconFileNameFromIndexTheme(const string &expectContext,
                                         const string &expectType, const string &mimeType, uint32_t iconSize) {
    for(auto &section : m_iniFile){
        string contextValue = section.second["Context"].as<string>();
        auto sizeValue = section.second["Size"].as<uint32_t>();
        string typeValue = section.second["Type"].as<string>();
        bool ret = (contextValue==expectContext && expectType == typeValue);
        if(iconSize!=-1){
            ret = (ret && iconSize == sizeValue);
        }
        if(ret){
            string fileName = ::GetIconFileName(m_iconFolder, section.first, mimeType);
            if(!fileName.empty()){
                return fileName;
            }
        }
    }
    return "";
}

#include "DirectoryIconTheme.h"
#include "FileIconTheme.h"

shared_ptr<IconTheme> IconThemeFactory::GetInstanceByFileName(const string &fileName) {
    static string iconThemeName = GetDesktopThemeName();
    static shared_ptr<IconTheme> directoryTheme = make_shared<DirectoryIconTheme>(iconThemeName);
    static shared_ptr<IconTheme>   fileIconTheme = make_shared<FileIconTheme>(iconThemeName);
    static bool initialize = false;
    if(!initialize){
        ParseThemeTree(directoryTheme.get());
        ParseThemeTree(fileIconTheme.get());
        initialize = true;
    }
    if(IsDirectory(fileName)){
        return directoryTheme;
    }
    return fileIconTheme;
}
