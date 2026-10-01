#pragma once
#include <windows.h>

struct WindowsInkPenInfo {
    UINT32 pointerId = 0;
    UINT32 pressure = 0;
    UINT32 rotation = 0;
    INT32 tiltX = 0;
    INT32 tiltY = 0;
    POINT pixelPosition{};
};

class WindowsInkPenInfoReader {
public:
    bool Read(UINT32 pointerId, WindowsInkPenInfo& out) const;
};
