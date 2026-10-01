#pragma once
#include "WindowsInkPointerCapture.h"
#include "WindowsInkPointerAdapter.h"
#include "WindowsInkHistoryAdapter.h"
#include <windows.h>
#include <vector>

class WindowsInkInputSession {
public:
    bool Begin(HWND hwnd, UINT32 pointerId, InkSample& sample);
    bool Update(HWND hwnd, UINT32 pointerId, std::vector<InkSample>& samples);
    bool End(HWND hwnd, UINT32 pointerId, InkSample& sample);
    void Reset();
private:
    HWND hwnd_ = nullptr;
    UINT32 pointerId_ = 0;
    bool active_ = false;
    WindowsInkPointerCapture capture_;
    WindowsInkPointerAdapter adapter_;
    WindowsInkHistoryAdapter history_;
};
