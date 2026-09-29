#pragma once
#include <windows.h>
#include "LayerSystem.h"

class LayerRenderer {
public:
    static void DrawBackground(HDC hdc, const RECT& viewport, COLORREF color = RGB(31, 34, 39));
    static void DrawInk(HDC hdc, const LayerSystem& layers);
};
