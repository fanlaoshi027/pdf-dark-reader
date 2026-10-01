#pragma once

#include <windows.h>
#include <cmath>
#include <algorithm>

// Shared viewport/page transform between the independent PDF reader and Mosuan.
// PDF and Mosuan keep their own data; only this transform is shared.
struct ViewTransform {
    double scale = 1.0;
    double originX = 0.0;
    double originY = 0.0;
    double pageWidth = 0.0;
    double pageHeight = 0.0;
    int pageIndex = 0;

    void Normalize() {
        if (!std::isfinite(scale) || scale <= 0.0) scale = 1.0;
        if (!std::isfinite(originX)) originX = 0.0;
        if (!std::isfinite(originY)) originY = 0.0;
        if (!std::isfinite(pageWidth) || pageWidth < 0.0) pageWidth = 0.0;
        if (!std::isfinite(pageHeight) || pageHeight < 0.0) pageHeight = 0.0;
        pageIndex = (std::max)(0, pageIndex);
    }

    POINT PageToView(double x, double y) const {
        const double s = (std::isfinite(scale) && scale > 0.0) ? scale : 1.0;
        POINT p{};
        p.x = static_cast<LONG>(std::lround(x * s + originX));
        p.y = static_cast<LONG>(std::lround(y * s + originY));
        return p;
    }

    POINT ViewToPage(int x, int y) const {
        const double s = (std::isfinite(scale) && scale > 0.0) ? scale : 1.0;
        POINT p{};
        p.x = static_cast<LONG>(std::lround((static_cast<double>(x) - originX) / s));
        p.y = static_cast<LONG>(std::lround((static_cast<double>(y) - originY) / s));
        return p;
    }
};
