#ifndef FILEDIALOG_PATHCHANGENOTIFY_H
#define FILEDIALOG_PATHCHANGENOTIFY_H
#include <string>

using namespace std;

class PathChangeNotify
{
public:
    virtual ~PathChangeNotify(){}
    virtual void PathChange(const string &path) = 0;
};

#endif //FILEDIALOG_PATHCHANGENOTIFY_H
