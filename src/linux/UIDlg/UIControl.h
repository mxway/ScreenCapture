#ifndef FILEDIALOG_UICONTROL_H
#define FILEDIALOG_UICONTROL_H
#include "X11Window.h"
#include "../../UIRect.h"

typedef struct _UIPoint
{
    int x;
    int y;
}UIPoint;

typedef Pixmap HDC;
typedef X11Window  *WindowHandle;

class UIControl {
public:
    explicit UIControl(UIRect rect = {0,0,0,0});
    virtual void OnPaint(HDC hdc);
    virtual void OnFocus();
    virtual void LostFocus();
    void            SetRect(const UIRect &rect);
    UIRect          GetRect()const;
    void            SetWindowHandle(WindowHandle windowHandle);
    WindowHandle    GetWindowHandle(){
        return m_winHandle;
    }
    virtual void            OnKeyDownEvent(MSG msg);
    bool IsPointIn(UIPoint point)const;
    void    Invalidate();
protected:
    UIRect m_rect;
    WindowHandle    m_winHandle;
};


#endif //FILEDIALOG_UICONTROL_H
