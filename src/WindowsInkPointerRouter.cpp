#include "WindowsInkPointerRouter.h"

bool WindowsInkPointerRouter::OnPointerDown(HWND hwnd, UINT32 pointerId, InkSample& sample) const {
    return WindowsInkPointerAdapter{}.Begin(hwnd, pointerId, sample);
}

bool WindowsInkPointerRouter::OnPointerUpdate(HWND hwnd, UINT32 pointerId, InkSample& sample) const {
    return WindowsInkPointerAdapter{}.Update(hwnd, pointerId, sample);
}

bool WindowsInkPointerRouter::OnPointerUp(HWND hwnd, UINT32 pointerId, InkSample& sample) const {
    return WindowsInkPointerAdapter{}.End(hwnd, pointerId, sample);
}
