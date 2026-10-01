#include "InkPointerSampleExtractor.h"
#include <windowsx.h>
#include <chrono>

InkSample InkPointerSampleExtractor::FromPointerUpdate(HWND hwnd, UINT32 pointerId) {
    const POINT p = PositionFromPointer(hwnd, pointerId);
    return {
        static_cast<double>(p.x),
        static_cast<double>(p.y),
        PressureFromPointer(pointerId),
        static_cast<std::uint64_t>(GetTickCount64())
    };
}

float InkPointerSampleExtractor::PressureFromPointer(UINT32 pointerId) {
    POINTER_PEN_INFO penInfo{};
    if (GetPointerPenInfo(pointerId, &penInfo) &&
        (penInfo.penFlags & PEN_FLAG_NONE) == PEN_FLAG_NONE) {
        return static_cast<float>(penInfo.pressure) / 1024.0f;
    }
    return 0.5f;
}

POINT InkPointerSampleExtractor::PositionFromPointer(HWND hwnd, UINT32 pointerId) {
    POINTER_INFO info{};
    POINT p{0, 0};
    if (GetPointerInfo(pointerId, &info)) {
        p = info.ptPixelLocation;
        ScreenToClient(hwnd, &p);
    }
    return p;
}
