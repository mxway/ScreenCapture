#include "Misc.h"
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <list>
#include "inicpp.h"
#include "IconTheme.h"
#include <magic.h>

static string GetThemeNameByCommand()
{
    char skinName[1024] = {0};
    FILE *fp = popen("gsettings get org.gnome.desktop.interface icon-theme","r");
    if(fp == nullptr){
        return "";
    }
    fread(skinName,1,1000,fp);
    fclose(fp);

    if(strlen(skinName) == 0){
        return "";
    }
    char *p = skinName;
    char *end = p + strlen(skinName) - 1;
    while (end>p && (*end=='\n' || *end =='\t' || *end=='\'' || *end=='\"')){
        *(end--) = '\0';
    }
    while(*p && (*p=='\n' || *p =='\t' || *p=='\'' || *p=='\"')){
        p++;
    }
    return p;
}

static string GetThemeNameFromSettings()
{
    string home = ::getenv("HOME");
    list<string>    gtkSettingFiles = {home + "/.config/gtk-2.0/settings.ini",
                                       home + "/.config/gtk-3.0/settings.ini",
                                       home + "/.config/gtk-4.0/settings.ini"};
    for(auto &settingFile: gtkSettingFiles){
        ini::IniFile iniFile;
        iniFile.load(settingFile);
        string iconThemeName = iniFile["Settings"]["gtk-icon-theme-name"].as<std::string>();
        if(!iconThemeName.empty()){
            return iconThemeName;
        }
    }
    return "";
}

static string GetGnomeDesktopThemeName(){
    string result = GetThemeNameByCommand();
    if(!result.empty() ){
        return result;
    }
    result = GetThemeNameFromSettings();
    if(!result.empty()){
        return result;
    }
    return "Adwaita";
}

static string GetNonGnomeDesktopThemeName(){
    string  result = GetThemeNameFromSettings();
    if(!result.empty()){
        return result;
    }
    result = GetThemeNameByCommand();
    if(!result.empty()){
        return result;
    }
    return "Adwaita";
}

static bool IsGnomeDesktop(){
    char gnome[] = "gnome";
    char *xdgCurrentDesktop = getenv("XDG_CURRENT_DESKTOP");
    if(xdgCurrentDesktop == nullptr){
        return false;
    }
    size_t cycleNumber = strlen(xdgCurrentDesktop) - strlen(gnome) + 1;
    for(int i=0;i<cycleNumber;i++){
        if(strncasecmp(xdgCurrentDesktop+i,gnome,5) == 0){
            return true;
        }
    }
    return false;
}

string GetDesktopThemeName(){
    if(IsGnomeDesktop()){
        return GetGnomeDesktopThemeName();
    }
    return GetNonGnomeDesktopThemeName();
}

void ParseThemeTree(IconTheme *current){
    string iconPath = "/usr/share/icons/" + current->GetIconThemeName() + "/index.theme";
    ini::IniFile iniFile;
    iniFile.load(iconPath);

    const char *parent_theme = iniFile["Icon Theme"]["Inherits"].as<const char*>();

    const char *p = parent_theme;
    const char *tok = strchr(parent_theme,',');
    while(tok != nullptr){
        string themeName;
        while(p < tok && *p){
            char name[2] = {*p++};
            themeName.append(name);
        }
        auto *parentTheme = current->Copy(themeName);
        current->PushParentTheme(parentTheme);
        ParseThemeTree(parentTheme);
        p++;
        tok = strchr(p,',');
    }
    //last parent theme
    if(*p){
        string themeName;
        while(*p){
            themeName.append(1,*p++);
        }
        auto *iconTheme = current->Copy(themeName);
        current->PushParentTheme(iconTheme);
        ParseThemeTree(iconTheme);
    }
}

bool EndWith(const string &str, const string &pattern,bool caseInsensitive)
{
    if(pattern.length()> str.length()){
        return false;
    }
    for(int i=0;i<pattern.length();i++){
        char c1 = pattern.at(i);
        char c2 = str.at(str.length() - pattern.length()+i);
        if(caseInsensitive){
            c1 = c1>='A'&&c1<='Z'?(char)(c1+0x20):c1;
            c2 = c2>='A' &&c2<='Z'?(char)(c2+0x20):c2;
        }
        if(c1!=c2){
            return false;
        }
    }
    return true;
}

bool IsDirectory(const string &fileName){
    struct stat data = {0};
    if(stat(fileName.c_str(),&data) == -1){
        return false;
    }
    return S_ISDIR(data.st_mode);
}

bool IsFileExists(const string &filename){
    return access(filename.c_str(),R_OK) == 0;
}

string GetFileContentType(const string &fileName){
    struct magic_set *cookie = NULL;
    cookie = magic_open(MAGIC_MIME);
    magic_load(cookie,"magic.mgc");
    string mimeType = magic_file(cookie,fileName.c_str());
    magic_close(cookie);
    int pos = mimeType.find(";");
    if(pos!=string::npos){
        return mimeType.substr(0,pos);
    }
    return mimeType;
}
