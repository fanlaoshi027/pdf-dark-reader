#pragma once
#include "LayerSystem.h"

struct StrokeStyle {
    COLORREF color = RGB(35,75,150);
    float baseWidth = 3.0f;
    bool dashed = false;
    bool oneStroke = true;
};

class StrokeDynamics {
public:
    static float ClampPressure(float pressure);
    static float WidthFromPressure(float pressure, float baseWidth);
    static float Speed(const InkPoint& a, const InkPoint& b);
    static void SmoothPoint(InkStroke& stroke, const InkPoint& point);
    static void ApplyTaper(InkStroke& stroke, float startRatio = 0.12f, float endRatio = 0.12f);
};
