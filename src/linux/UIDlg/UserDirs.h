#ifndef FILEDIALOG_USERDIRS_H
#define FILEDIALOG_USERDIRS_H
#include <string>

using namespace std;

enum SpecialUserDir{
    SpecialUserDir_Desktop,
    SpecialUserDir_Download,
    SpecialUserDir_Templates,
    SpecialUserDir_PublicShare,
    SpecialUserDir_Documents,
    SpecialUserDir_Music,
    SpecialUserDir_Pictures,
    SpecialUserDir_Videos,
    SpecialUserDir_Unknown
};

class UserDirs {
public:
    static UserDirs &GetInstance();
    string  GetUserSpecialDir(SpecialUserDir userDir);
private:
    UserDirs();
};


#endif //FILEDIALOG_USERDIRS_H
