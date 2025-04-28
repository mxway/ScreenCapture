#ifndef FILEDIALOG_MISC_H
#define FILEDIALOG_MISC_H
#include <string>
using namespace std;

class IconTheme;

string GetDesktopThemeName();

void ParseThemeTree(IconTheme *current);

bool EndWith(const string &str, const string &pattern,bool caseInsensitive=false);

bool IsDirectory(const string &fileName);

bool IsFileExists(const string &filename);

string GetFileContentType(const string &fileName);


#endif //FILEDIALOG_MISC_H
