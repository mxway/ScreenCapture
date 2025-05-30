#ifndef FILEDIALOG_FILEDIALOG_H
#define FILEDIALOG_FILEDIALOG_H
#include "XBaseWindow.h"
#include "UIFileListControl.h"
#include "UIPathButton.h"
#include "UIEdit.h"
#include "PathChangeNotify.h"

class FileDialog : public XBaseWindow, public PathChangeNotify{
public:
    FileDialog();
    ~FileDialog();
    long OnReSize(MSG msg) override;

    long OnPaint(MSG msg) override;

    long OnMouseWheel(MSG msg) override;

    long OnLeftButtonPress(MSG msg) override;

    long OnLeftButtonRelease(MSG msg) override;

    long OnMouseMove(MSG msg) override;

    void PathChange(const string &path) override;

    long OnKeyPress(MSG msg) override;

    long OnClose(MSG msg) override;

    long OnCreate() override;

    string   GetFileName();

private:
    void    Invalidate();
    void    ChangeWindowStyle();
private:
    UIFileListControl m_fileList;
    UIPathButton      m_pathButton;
    UIEdit            m_edit;
    Cursor            m_insertionCursor;
    Cursor            m_normalCursor;
};


#endif //FILEDIALOG_FILEDIALOG_H
