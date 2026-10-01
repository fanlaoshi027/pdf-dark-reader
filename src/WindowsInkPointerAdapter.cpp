#include "WindowsInkPointerAdapter.h"
#include <windowsx.h>
#include <cmath>

namespace {
InkSample MakeSample(POINTER_PEN_INFO& info) {
    InkSample sample{};
    sample.x = static_cast<double>(info.pointerInfo.ptPixelLocation.x);
    sample.y = static_cast<double>(info.pointerInfo.ptPixelLocation.y);
    sample.pressure = static_cast<double>(info.pressure) / 1024.0;
    sample.timestamp = static_cast<double>(info.pointerInfo.dwTime) / 1000.0;
    return sample;
}
}

bool WindowsInkPointerAdapter::Read(HWND hwnd, UINT32 pointerId, InkSample& out) const {
    POINTER_INPUT_TYPE type{};
    if (!GetPointerType(pointerId, &type) || type != PT_PEN) return false;
    POINTER_PEN_INFO info{};
    if (!GetPointerPenInfo(pointerId, &info)) return false;
    out = MakeSample(info);
    return true;
}

bool WindowsInkPointerAdapter::Begin(HWND hwnd, UINT32 pointerId, InkSample& out) const {
    return Read(hwnd, pointerId, out);
}

bool WindowsInkPointerAdapter::Update(HWND hwnd, UINT32 pointerId, InkSample& out) const {
    return Read(hwnd, pointerId, out);
}

bool WindowsInkPointerAdapter::End(HWND hwnd, UINT32 pointerId, InkSample& out) const {
    return Read(hwnd, pointerId, out);
}
