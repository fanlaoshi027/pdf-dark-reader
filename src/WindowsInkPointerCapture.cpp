#include "WindowsInkPointerCapture.h"

bool WindowsInkPointerCapture::Capture(HWND hwnd, UINT32 pointerId) const {
    return hwnd != nullptr && SetPointerCapture(hwnd, pointerId);
}

void WindowsInkPointerCapture::Release(HWND hwnd, UINT32 pointerId) const {
    if (hwnd) ReleasePointerCapture(hwnd, pointerId);
}
