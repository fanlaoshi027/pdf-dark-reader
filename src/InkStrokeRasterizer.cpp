#include "InkStrokeRasterizer.h"
#include <windows.h>
#include <algorithm>
#include <cmath>

void InkStrokeRasterizer::Draw(HDC dc, const VectorStroke& stroke) const {
    if (!dc || stroke.points.empty()) return;
    const int width = std::max(1, static_cast<int>(std::round(stroke.width)));
    const COLORREF color = RGB((stroke.color >> 16) & 0xff, (stroke.color >> 8) & 0xff, stroke.color & 0xff);
    HPEN pen = CreatePen(stroke.dashed ? PS_DASH : PS_SOLID, width, color);
    if (!pen) return;
    const HGDIOBJ old = SelectObject(dc, pen);
    MoveToEx(dc, static_cast<int>(stroke.points.front().x), static_cast<int>(stroke.points.front().y), nullptr);
    for (std::size_t i = 1; i < stroke.points.size(); ++i) {
        LineTo(dc, static_cast<int>(stroke.points[i].x), static_cast<int>(stroke.points[i].y));
    }
    SelectObject(dc, old);
    DeleteObject(pen);
}
