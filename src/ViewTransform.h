#pragma once

#include <windows.h>

// Shared viewport/page transform between the independent PDF reader and Mosuan.
// PDF and Mosuan keep their own data; only this transform is shared.
struct ViewTransform {
    double scale = 1.0;
    double offsetX = 0.0;
    double offsetY = 0.0;
    double pageWidth = 0.0;
    double pageHeight = 0.0;

    POINT PageToView(double x, double y) const {
        POINT p{};
        p.x = static_cast<LONG>(x * scale + offsetX + 0.5);
        p.y = static_cast<LONG>(y * scale + offsetY + 0.5);
        return p;
    }

    POINT ViewToPage(int x, int y) const {
        POINT p{};
        const double s = scale > 0.0 ? scale : 1.0;
        p.x = static_cast<LONG>((x - offsetX) / s + 0.5);
        p.y = static_cast<LONG>((y - offsetY) / s + 0.5);
        return p;
    }
};
