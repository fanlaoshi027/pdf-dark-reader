#pragma once
#include "WindowsInkPointerAdapter.h"
#include <windows.h>

class WindowsInkPointerRouter {
public:
    bool OnPointerDown(HWND hwnd, UINT32 pointerId, InkSample& sample) const;
    bool OnPointerUpdate(HWND hwnd, UINT32 pointerId, InkSample& sample) const;
    bool OnPointerUp(HWND hwnd, UINT32 pointerId, InkSample& sample) const;
};
