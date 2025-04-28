#include "UIControl.h"

UIControl::UIControl(UIRect rect)
    :m_rect{rect}
{

}

void UIControl::OnPaint(HDC hdc) {

}

void UIControl::OnFocus() {

}

void UIControl::LostFocus() {

}

void UIControl::SetRect(const UIRect &rect) {
    m_rect = rect;
}

UIRect UIControl::GetRect() const {
    return m_rect;
}

void UIControl::OnKeyDownEvent(MSG msg) {

}

void UIControl::SetWindowHandle(WindowHandle windowHandle) {
    m_winHandle = windowHandle;
}

bool UIControl::IsPointIn(UIPoint point) const {
    return point.x >= m_rect.x
                && point.x < m_rect.x + m_rect.width
                &&point.y >= m_rect.y
                &&point.y < m_rect.y + m_rect.height;
}

void UIControl::Invalidate() {
    X11Window  *window = this->GetWindowHandle();
    XEvent event;
    event.type = Expose;
    event.xexpose.window = window->window;
    event.xexpose.x = m_rect.x;
    event.xexpose.y = m_rect.y;
    event.xexpose.width = m_rect.width;
    event.xexpose.height = m_rect.height;
    event.xexpose.count = 0;

    XSendEvent(window->display, window->window, False, ExposureMask, &event);
}
