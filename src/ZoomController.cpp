#include "ZoomController.h"

void ZoomController::Multiply(double factor, double centerX, double centerY) {
    if (factor <= 0.0) return;
    const auto current = transform_.State().scale;
    transform_.ZoomAround(current * factor, centerX, centerY);
}

void ZoomController::Set(double scale, double centerX, double centerY) {
    transform_.ZoomAround(scale, centerX, centerY);
}
