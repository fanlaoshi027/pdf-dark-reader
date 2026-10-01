#include "PdfViewStateController.h"
#include <algorithm>
#include <cmath>

void PdfViewStateController::Reset(int pageCount) {
    state_ = {};
    state_.pageCount = std::max(0, pageCount);
}

void PdfViewStateController::SetPage(int pageIndex) {
    if (state_.pageCount <= 0) {
        state_.pageIndex = 0;
        return;
    }
    state_.pageIndex = std::clamp(pageIndex, 0, state_.pageCount - 1);
}

void PdfViewStateController::SetRenderSize(int width, int height) {
    state_.renderWidth = std::max(0, width);
    state_.renderHeight = std::max(0, height);
}

void PdfViewStateController::SetZoom(double zoom) {
    if (!std::isfinite(zoom)) return;
    state_.zoom = std::clamp(zoom, 0.1, 8.0);
}

void PdfViewStateController::SetScrollY(int scrollY) {
    state_.scrollY = std::max(0, scrollY);
}

void PdfViewStateController::SetFitWidth(bool enabled) {
    state_.fitWidth = enabled;
}

void PdfViewStateController::SetInvert(bool enabled) {
    state_.invert = enabled;
}
