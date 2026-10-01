#include "ViewTransformController.h"
#include "ViewTransformMath.h"
#include <cmath>

void ViewTransformController::SetPage(int pageIndex, double width, double height) {
    state_.pageIndex = pageIndex < 0 ? 0 : pageIndex;
    state_.pageWidth = width > 0.0 ? width : 0.0;
    state_.pageHeight = height > 0.0 ? height : 0.0;
    state_.offsetX = 0.0;
    state_.offsetY = 0.0;
}

void ViewTransformController::Pan(double dx, double dy) {
    if (!std::isfinite(dx) || !std::isfinite(dy)) return;
    state_.offsetX += dx;
    state_.offsetY += dy;
}

void ViewTransformController::ZoomAround(double newScale, double screenX, double screenY) {
    if (!std::isfinite(screenX) || !std::isfinite(screenY)) return;
    ViewTransformMath::ZoomAroundScreenPoint(state_, newScale, screenX, screenY);
}

void ViewTransformController::SetScale(double scale) {
    if (!std::isfinite(scale)) return;
    state_.scale = ViewTransformMath::ClampScale(scale, 0.1, 8.0);
}

void ViewTransformController::ResetOrigin() {
    state_.offsetX = 0.0;
    state_.offsetY = 0.0;
}
