
#ifndef CHARSETCONVERT_H
#define CHARSETCONVERT_H

#include <windows.h>

wchar_t *Utf8ToUtf16(const char *utf8String, int length);

char *Utf16ToUtf8(const wchar_t *utf16String, int length);

#endif //CHARSETCONVERT_H
