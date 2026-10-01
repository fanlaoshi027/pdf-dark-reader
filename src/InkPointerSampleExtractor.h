#pragma once

#include "InkSample.h"
#include <windows.h>

class InkPointerSampleExtractor {
public:
    static InkSample FromPointerUpdate(HWND hwnd, UINT32 pointerId);
    static float PressureFromPointer(UINT32 pointerId);
    static POINT PositionFromPointer(HWND hwnd, UINT32 pointerId);
};
