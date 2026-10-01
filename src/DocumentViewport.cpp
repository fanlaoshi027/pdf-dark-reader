#include "DocumentViewport.h"
#include <algorithm>
#include <cmath>

void DocumentViewport::SetZoom(double zoom) {
    if (!std::isfinite(zoom)) return;
    zoom_ = std::clamp(zoom, 0.1, 8.0);
}

void DocumentViewport::SetOffset(double x, double y) {
    if (std::isfinite(x)) offsetX_ = x;
    if (std::isfinite(y)) offsetY_ = y;
}

void DocumentViewport::Pan(double dx, double dy) {
    if (std::isfinite(dx)) offsetX_ += dx;
    if (std::isfinite(dy)) offsetY_ += dy;
}

ViewPoint DocumentViewport::DocumentToScreen(ViewPoint document) const {
    return {document.x * zoom_ + offsetX_,
            document.y * zoom_ + offsetY_};
}

ViewPoint DocumentViewport::ScreenToDocument(ViewPoint screen) const {
    return {(screen.x - offsetX_) / zoom_,
            (screen.y - offsetY_) / zoom_};
}
