#include "LayerTransform.h"
#include <algorithm>
#include <cmath>

POINT LayerTransform::PdfToView(const PdfViewTransform& t, double x, double y) {
    POINT p{};
    p.x = t.originX + static_cast<LONG>(std::lround(x * t.scale));
    p.y = t.originY + static_cast<LONG>(std::lround(y * t.scale));
    return p;
}

POINT LayerTransform::ViewToPdf(const PdfViewTransform& t, int x, int y) {
    POINT p{};
    const double scale = ClampScale(t.scale);
    p.x = static_cast<LONG>(std::lround((x - t.originX) / scale));
    p.y = static_cast<LONG>(std::lround((y - t.originY) / scale));
    return p;
}

double LayerTransform::ClampScale(double scale) {
    return std::clamp(scale, 0.10, 8.0);
}
