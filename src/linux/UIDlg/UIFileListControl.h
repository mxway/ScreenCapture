#ifndef FILEDIALOG_UIFILELISTCONTROL_H
#define FILEDIALOG_UIFILELISTCONTROL_H
#include "UIControl.h"
#include "PathChangeNotify.h"
#include <string>
#include <vector>


using namespace std;

class UIFileListControl :public UIControl{
public:
    explicit UIFileListControl();
    void OnPaint(HDC hdc) override;
    void SetPath(const string &path);
    void OnLeftButtonPressed(MSG msg);
    void OnLeftMouseRelease(MSG msg);
    void OnMouserWheel(MSG msg);
    void OnMouseMove(MSG msg);

    void OnFocus() override;

    void LostFocus() override;

    void SetPathChangeNotify(PathChangeNotify *pathChangeNotify){
        m_pathChangeNotify = pathChangeNotify;
    }

    string GetPath()const;
private:
    void    PaintFileList(HDC hdc);
    void    PaintScrollBar(HDC hdc);
    void    LoadSubFiles();
    void    ScrollUp(int step = 1);
    void    ScrollDown(int step = 1);
    void    DoDoubleClick(MSG msg);
    void    DolFileSelectionEvent(MSG msg);
    void    DoScrollBarEvent(MSG msg);
private:
    int m_curSel;
    PathChangeNotify *m_pathChangeNotify;
    int m_startDrawItem;
    int m_itemHeight;
    string  m_currentDir;
    UIRect  m_thumbRect;
    vector<string>  m_files;
};


#endif //FILEDIALOG_UIFILELISTCONTROL_H
