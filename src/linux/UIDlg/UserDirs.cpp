#include "UserDirs.h"
#include <map>
#include <cstring>

using namespace std;

#define MAX_LINE_LENGTH 1024

static map<string,string> userSpecialFoldersMapping;

UserDirs &UserDirs::GetInstance() {
    static UserDirs userDirs;
    return userDirs;
}

string UserDirs::GetUserSpecialDir(SpecialUserDir userDir) {
    static string userDirId2String[SpecialUserDir_Unknown] = {
            "XDG_DESKTOP_DIR",
            "XDG_DOWNLOAD_DIR",
            "XDG_TEMPLATES_DIR",
            "XDG_PUBLICSHARE_DIR",
            "XDG_DOCUMENTS_DIR",
            "XDG_MUSIC_DIR",
            "XDG_PICTURES_DIR",
            "XDG_VIDEOS_DIR"
    };
    if(userDir>=SpecialUserDir_Unknown || userDir<SpecialUserDir_Desktop){
        return "";
    }
    auto itr = userSpecialFoldersMapping.find(userDirId2String[userDir]);
    if(itr == userSpecialFoldersMapping.end()){
        return "";
    }
    return itr->second;
}

void parse_line(const char *line, char *key, char *value) {
    // skip empty line and comment line
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
        key[0] = '\0';
        value[0] = '\0';
        return;
    }

    const char *equal_sign = strchr(line, '=');
    if (!equal_sign) {
        key[0] = '\0';
        value[0] = '\0';
        return;
    }

    // fetch key
    int key_len = equal_sign - line;
    strncpy(key, line, key_len);
    key[key_len] = '\0';

    // remove quote
    const char *value_start = equal_sign + 1;
    while (*value_start == ' ' || *value_start == '"') value_start++;
    const char *value_end = value_start;
    while (*value_end != '"' && *value_end != '\0' && *value_end != '\n') value_end++;
    int value_len = value_end - value_start;
    strncpy(value, value_start, value_len);
    value[value_len] = '\0';
}

void replace_env_vars(char *value, size_t max_len) {
    char expanded_value[MAX_LINE_LENGTH] = {0};
    char *src = value;
    char *dst = expanded_value;

    while (*src && dst - expanded_value < max_len - 1) {
        if (*src == '$') {
            src++;
            char env_var_name[MAX_LINE_LENGTH];
            char *env_var_ptr = env_var_name;

            // fetch env var name
            while (*src && (*src != '/' && *src != '"')) {
                *env_var_ptr++ = *src++;
            }
            *env_var_ptr = '\0';

            // fetch env value
            const char *env_value = getenv(env_var_name);
            if (env_value) {
                strncpy(dst, env_value, max_len - (dst - expanded_value));
                dst += strlen(env_value);
            }
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    strncpy(value, expanded_value, max_len);
}

UserDirs::UserDirs() {
    char userDirsFileName[1024] = {0};
    snprintf(userDirsFileName, 1000, "%s/.config/user-dirs.dirs", getenv("HOME"));
    FILE *fp = fopen(userDirsFileName, "r");
    if(fp == nullptr){
        return;
    }
    char line[MAX_LINE_LENGTH+2] = {0};
    char key[MAX_LINE_LENGTH+2] = {0};
    char value[MAX_LINE_LENGTH+2] = {0};

    while (fgets(line, sizeof(line), fp)) {

        parse_line(line, key, value);

        if (key[0] != '\0' && value[0] != '\0') {
            replace_env_vars(value, MAX_LINE_LENGTH);
            userSpecialFoldersMapping.insert(make_pair(key,value));
        }
    }
    fclose(fp);
}
