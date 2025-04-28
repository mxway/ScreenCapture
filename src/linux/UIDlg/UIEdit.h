#ifndef FILEDIALOG_UIEDIT_H
#define FILEDIALOG_UIEDIT_H
#include "UIControl.h"
#include <string>
#include <X11/Xft/Xft.h>

using namespace std;

#if !defined(CharNext)
inline const char* CharNext(const char *p){
    u_char  character = *p;
    if(*p == 0){
        return p;
    }
    if( (character & 0x80) == 0){
        return (p+1);
    }else if( (character >> 5) == 0B110){
        return (p+2);
    }else if( (character>>4) == 0B1110){
        return (p+3);
    }else if( (character>>3) == 0B11110){
        return (p+4);
    }else if( (character>>2) == 0B111110){
        return (p+5);
    }else if( (character>>1) == 0B1111110){
        return (p+6);
    }
    return p+1;
}
#endif

#if !defined(CharPrev)
inline const char* CharPrev(const char *start, const char *current)
{
    if(start == current){
        return start;
    }
    const char *result = current - 1;
    while(result != start){
        u_char character = *result;
        if( (character>>6) == 0B10){
            result = result - 1;
        }else{
            break;
        }
    }
    return result;
}
#endif

class UIEdit : public UIControl{
public:
    UIEdit();
    ~UIEdit();
    void OnPaint(HDC hdc) override;

    void OnFocus() override;

    void LostFocus() override;

    void OnKeyDownEvent(MSG msg) override;

    string GetText()const{
        return m_text;
    }

private:
    void DrawInsertionCursor(HDC hdc);

private:
    void *m_ic;
    string      m_text;
    XftFont     *m_font;
    Pixmap      m_cursor;
    bool        m_focusState;
};


#endif //FILEDIALOG_UIEDIT_H
