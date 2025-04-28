#include <cstring>
#include "UIPathButton.h"
#include <X11/Xft/Xft.h>
#include <iostream>

using namespace std;

typedef struct PathButtonRect_s
{
    int x;
    int y;
    int width;
    int height;
}PathButtonRect;

vector<PathButtonRect>  glbButtonPaths;

const int PATH_SEP_WIDTH = 15;

UIPathButton::UIPathButton()
    :m_pathChangeNotify{nullptr}
{
}

void UIPathButton::SetPath(const string &path) {
    char *realPath = realpath(path.c_str(), nullptr);
    m_paths.clear();
    glbButtonPaths.clear();
    m_path = realPath;
    m_paths.push_back("/");
    char *p = realPath+1;
    char *tok = strchr(p,'/');
    while(tok != nullptr){
        m_paths.push_back(string(p,tok-p));
        p = tok + 1;
        tok = strchr(p,'/');
    }
    if(p && *p){
        m_paths.push_back(p);
    }
    free(realPath);
}

void UIPathButton::OnPaint(HDC hdc) {
    UIControl::OnPaint(hdc);
    X11Window *window = this->GetWindowHandle();
    XftFont *font = XftFontOpenName(window->display, window->screen,"Default:size=12");
    XGlyphInfo  glyphInfo;
    int x = m_rect.x;
    int y = m_rect.y;
    glbButtonPaths.clear();
    XftDraw *draw = XftDrawCreate(window->display, hdc, window->visual,
                                  window->colormap);
    GC gc = XCreateGC(window->display,hdc, 0, nullptr);
    for(auto &path :m_paths){
        XftTextExtentsUtf8(window->display, font, reinterpret_cast<const FcChar8 *>(path.c_str()), path.length(), &glyphInfo);
        XSetForeground(window->display,gc,0xffc1c1c1);
        XDrawRectangle(window->display,hdc,gc,x,y+4,glyphInfo.width+8,m_rect.height-8);
        XSetForeground(window->display,gc, 0xfff1f1f1);
        XFillRectangle(window->display,hdc, gc,x+1,y+5,glyphInfo.width+7,m_rect.height-9);
        XftColor textColor;
        textColor.color.alpha = 0xffff;
        textColor.color.red = 0;
        textColor.color.green = 0;
        textColor.color.blue = 0;
        XftDrawStringUtf8(draw, &textColor, font,x+4,y+font->ascent, reinterpret_cast<const FcChar8 *>(path.c_str()), path.length());
        PathButtonRect buttonRect;
        buttonRect.x = x;
        buttonRect.y = y;
        buttonRect.width = glyphInfo.width + 8;
        buttonRect.height = m_rect.height-8;
        glbButtonPaths.push_back(buttonRect);
        ///XftDrawRect(draw,&lineColor,x,y+4,glyphInfo.width,m_rect.height-8);
        x += PATH_SEP_WIDTH + glyphInfo.width;

    }
    XFreeGC(window->display,gc);
    XftDrawDestroy(draw);
    XftFontClose(window->display,font);
}

void UIPathButton::OnLeftButtonPressed(MSG msg) {
    UIPoint point = {msg.xbutton.x,msg.xbutton.y};
    if(!IsPointIn(point)){
        return;
    }
    int buttonRectIndex = 0;
    for(buttonRectIndex=0; buttonRectIndex < glbButtonPaths.size(); buttonRectIndex++){
        auto &buttonRect = glbButtonPaths[buttonRectIndex];
        if(point.x>=buttonRect.x && point.x<=buttonRect.x+buttonRect.width
           && point.y>=buttonRect.y && point.y<=buttonRect.y + buttonRect.height){
            break;
        }
    }
    if(buttonRectIndex >= glbButtonPaths.size()-1  || m_pathChangeNotify== nullptr){
        return;
    }
    string path{"/"};
    for(int i=1;i<=buttonRectIndex;i++){
        if(i>1){
            path.append("/");
        }
        path.append(m_paths[i]);
    }
    m_pathChangeNotify->PathChange(path);
}

void UIPathButton::OnFocus() {
    UIControl::OnFocus();
}

void UIPathButton::LostFocus() {
    UIControl::LostFocus();
}
