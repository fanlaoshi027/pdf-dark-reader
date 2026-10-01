#pragma once
#include "InkSample.h"
#include <windows.h>

class WindowsInkPointerAdapter {
public:
    bool Begin(HWND hwnd, UINT32 pointerId, InkSample& out) const;
    bool Update(HWND hwnd, UINT32 pointerId, InkSample& out) const;
    bool End(HWND hwnd, UINT32 pointerId, InkSample& out) const;
private:
    bool Read(HWND hwnd, UINT32 pointerId, InkSample& out) const;
};
