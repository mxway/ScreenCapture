#include "CharsetConvert.h"

#include <cstdint>

wchar_t *Utf8ToUtf16(const char *utf8String, int length){
    if (utf8String == nullptr) {
        return nullptr;
    }
    uint32_t utf8StringLength = length<=0?strlen(utf8String):length;
    const int requiredNumberOfWideChar = ::MultiByteToWideChar(CP_UTF8,0,utf8String,utf8StringLength,0,0);
    auto *wideCharBuffer = new wchar_t[requiredNumberOfWideChar+2];
    memset(wideCharBuffer, 0, (requiredNumberOfWideChar+2)*sizeof(wchar_t));
    ::MultiByteToWideChar(CP_UTF8, 0, utf8String, utf8StringLength, wideCharBuffer, requiredNumberOfWideChar);
    return wideCharBuffer;
}

char *Utf16ToUtf8(const wchar_t *utf16String, int length){
    if (utf16String == nullptr) {
        return nullptr;
    }
    uint32_t wideStringLength = length<=0?wcslen(utf16String):length;
    uint32_t requiredNumberOfChar = ::WideCharToMultiByte(CP_UTF8,0,utf16String,wideStringLength,0,0,0,0);
    char *utf8String = new char[requiredNumberOfChar+1];
    memset(utf8String, 0, (requiredNumberOfChar+1)*sizeof(char));
    ::WideCharToMultiByte(CP_UTF8, 0, utf16String, wideStringLength, utf8String, requiredNumberOfChar, 0, 0);
    return utf8String;
}