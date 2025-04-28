#ifndef FILEDIALOG_XBASEWINDOW_H
#define FILEDIALOG_XBASEWINDOW_H
#include "X11Window.h"

enum WindowType
{
    WindowType_TopLevel,
    WindowType_Popup
};

enum UIDlgButtonId
{
    UI_DLG_BUTTON_OK,
    UI_DLG_BUTTON_CANCEL,
    UI_DLG_BUTTON_SAVE,
    UI_DLG_BUTTON_YES,
    UI_DLG_BUTTON_NO,
    UI_DLG_BUTTON_CLOSE
};

class XBaseWindow {
public:
    XBaseWindow();
    virtual ~XBaseWindow();

    bool        Create(int x,int y,int width,int height,const char *title,Window parent = 0,WindowType windowType = WindowType_TopLevel);

    void        SetTitle(const char *title);

    X11Window   *GetWindowHandle(){
        return m_x11Window;
    }

    void   ShowWindow();

    void   Close(UIDlgButtonId id = UI_DLG_BUTTON_CLOSE);

    void MessageLoop();

    virtual void OnFinalMessage();

    virtual long OnPaint(MSG msg);
    virtual long OnCreate();
    virtual long OnReSize(MSG msg);
    virtual long OnLeftButtonPress(MSG msg);
    virtual long OnLeftButtonRelease(MSG msg);
    virtual long OnRightButtonPress(MSG msg);
    virtual long OnRightButtonRelease(MSG msg);
    virtual long OnKeyPress(MSG msg);
    virtual long OnKeyRelease(MSG msg);
    virtual long OnClose(MSG msg);
    virtual long OnMouseMove(MSG msg);
    virtual long OnMouseWheel(MSG msg);

    void    MessageHandler(MSG msg);

protected:
    X11Window *m_x11Window;

};


#endif //FILEDIALOG_XBASEWINDOW_H
