#ifndef SCREENCAPTURE_MAINWINDOW_H
#define SCREENCAPTURE_MAINWINDOW_H
#include "XBaseWindow.h"

class MainWindow : public XBaseWindow{
public:
    MainWindow();
    ~MainWindow();
    bool    CreateFullScreenWindow();
    long OnPaint(MSG msg) override;

    long OnCreate() override;

    long OnReSize(MSG msg) override;

    long OnLeftButtonPress(MSG msg) override;

    long OnLeftButtonRelease(MSG msg) override;

    long OnRightButtonPress(MSG msg) override;

    long OnKeyPress(MSG msg) override;

    long OnKeyRelease(MSG msg) override;

    long OnClose(MSG msg) override;

    long OnMouseMove(MSG msg) override;

    long OnMouseWheel(MSG msg) override;
private:
    void    ChangeWindowStyle();
    void    CreateRootWindowPixmap();
    void    CreateDarkPixmap();
private:
    Pixmap      m_rootWindowPixmap;
    Pixmap      m_darkPixmap;
    GC          m_gc;
    Cursor      m_selectionCursor;
};


#endif //SCREENCAPTURE_MAINWINDOW_H
