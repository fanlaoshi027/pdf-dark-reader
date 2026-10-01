#pragma once

#include "ViewTransformState.h"

namespace ViewTransformMath {

inline double ClampScale(double value, double minScale, double maxScale) {
    if (value < minScale) return minScale;
    if (value > maxScale) return maxScale;
    return value;
}

inline double ScreenToPageX(const ViewTransformState& s, double x) {
    return (x - s.offsetX) / s.scale;
}

inline double ScreenToPageY(const ViewTransformState& s, double y) {
    return (y - s.offsetY) / s.scale;
}

inline double PageToScreenX(const ViewTransformState& s, double x) {
    return x * s.scale + s.offsetX;
}

inline double PageToScreenY(const ViewTransformState& s, double y) {
    return y * s.scale + s.offsetY;
}

inline void ZoomAroundScreenPoint(ViewTransformState& s, double newScale,
                                  double screenX, double screenY,
                                  double minScale = 0.1,
                                  double maxScale = 8.0) {
    newScale = ClampScale(newScale, minScale, maxScale);
    if (s.scale <= 0.0) s.scale = 1.0;

    const double pageX = ScreenToPageX(s, screenX);
    const double pageY = ScreenToPageY(s, screenY);

    s.scale = newScale;
    s.offsetX = screenX - pageX * s.scale;
    s.offsetY = screenY - pageY * s.scale;
}

} // namespace ViewTransformMath
