#ifndef FILEDIALOG_UIPATHBUTTON_H
#define FILEDIALOG_UIPATHBUTTON_H
#include "UIControl.h"
#include <string>
#include <vector>
#include "PathChangeNotify.h"

using namespace std;

class UIPathButton : public UIControl{
public:
    explicit UIPathButton();
    void     SetPath(const string &path);

    void OnPaint(HDC hdc) override;

    void OnLeftButtonPressed(MSG msg);

    void OnFocus() override;

    void LostFocus() override;

    void SetPathChangeNotify(PathChangeNotify *pathChangeNotify){
        m_pathChangeNotify = pathChangeNotify;
    }

private:
    PathChangeNotify    *m_pathChangeNotify;
    string          m_path;
    vector<string>  m_paths;
};

#endif //FILEDIALOG_UIPATHBUTTON_H
