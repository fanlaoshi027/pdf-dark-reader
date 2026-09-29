#include "StrokeRenderer.h"
#include "LayerTransform.h"
#include <algorithm>
#include <cmath>

void StrokeRenderer::Draw(HDC hdc, const InkStroke& stroke, const PdfViewTransform& transform, const StrokeStyle& style) {
    if (stroke.points.empty()) return;
    if (stroke.points.size() == 1) {
        const POINT p = LayerTransform::PdfToView(transform, stroke.points[0].pdfX, stroke.points[0].pdfY);
        const int r = std::max(1, static_cast<int>(std::lround(StrokeDynamics::WidthFromPressure(stroke.points[0].pressure, style.baseWidth) * 0.5f)));
        HBRUSH brush = CreateSolidBrush(style.color);
        Ellipse(hdc, p.x-r, p.y-r, p.x+r, p.y+r);
        DeleteObject(brush);
        return;
    }

    HPEN pen = CreatePen(style.dashed ? PS_DASH : PS_SOLID,
                         std::max(1, static_cast<int>(std::lround(style.baseWidth))), style.color);
    HGDIOBJ old = SelectObject(hdc, pen);
    POINT p = LayerTransform::PdfToView(transform, stroke.points[0].pdfX, stroke.points[0].pdfY);
    MoveToEx(hdc, p.x, p.y, nullptr);
    for (size_t i = 1; i < stroke.points.size(); ++i) {
        p = LayerTransform::PdfToView(transform, stroke.points[i].pdfX, stroke.points[i].pdfY);
        LineTo(hdc, p.x, p.y);
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

void StrokeRenderer::DrawLine(HDC hdc, POINT a, POINT b, const StrokeStyle& style) {
    HPEN pen = CreatePen(style.dashed ? PS_DASH : PS_SOLID,
                         std::max(1, static_cast<int>(std::lround(style.baseWidth))), style.color);
    HGDIOBJ old = SelectObject(hdc, pen);
    MoveToEx(hdc, a.x, a.y, nullptr);
    LineTo(hdc, b.x, b.y);
    SelectObject(hdc, old);
    DeleteObject(pen);
}
