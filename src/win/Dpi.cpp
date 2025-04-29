#include "Dpi.h"
#include <windows.h>

typedef HRESULT (WINAPI *SetProcessDpiAwareness)(int);

void Dpi::SetDpiAwareness(ProcessDpiAwareness dpiAwareness) {
    HMODULE moduleHandle = ::LoadLibraryA("Shcore.dll");
    if (moduleHandle == INVALID_HANDLE_VALUE || moduleHandle == nullptr) {
        return;
    }
    auto setProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwareness>(GetProcAddress(moduleHandle, "SetProcessDpiAwareness"));
    if (setProcessDpiAwareness == nullptr) {
        ::FreeLibrary(moduleHandle);
        return;
    }
    setProcessDpiAwareness(dpiAwareness);
    ::FreeLibrary(moduleHandle);
}
