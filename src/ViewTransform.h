#pragma once

#include <windows.h>
#include <cmath>

// Shared viewport/page transform between the independent PDF reader and Mosuan.
// PDF and Mosuan keep their own data; only this transform is shared.
struct ViewTransform {
    double scale = 1.0;
    double offsetX = 0.0;
    double offsetY = 0.0;
    double pageWidth = 0.0;
    double pageHeight = 0.0;
    int pageIndex = 0;

    POINT PageToView(double x, double y) const {
        return POINT{
            static_cast<LONG>(std::lround(x * scale + offsetX)),
            static_cast<LONG>(std::lround(y * scale + offsetY))
        };
    }

    POINT ViewToPage(int x, int y) const {
        const double s = scale > 0.0 ? scale : 1.0;
        return POINT{
            static_cast<LONG>(std::lround((x - offsetX) / s)),
            static_cast<LONG>((y - offsetY) / s)
        };
    }
};
