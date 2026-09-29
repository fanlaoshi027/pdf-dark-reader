#pragma once
#include <windows.h>
#include "LayerSystem.h"
#include "StrokeDynamics.h"

class StrokeRenderer {
public:
    static void Draw(HDC hdc, const InkStroke& stroke, const PdfViewTransform& transform, const StrokeStyle& style);
    static void DrawLine(HDC hdc, POINT a, POINT b, const StrokeStyle& style);
};
