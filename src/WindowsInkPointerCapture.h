#pragma once
#include <windows.h>

class WindowsInkPointerCapture {
public:
    bool Capture(HWND hwnd, UINT32 pointerId) const;
    void Release(HWND hwnd, UINT32 pointerId) const;
};
