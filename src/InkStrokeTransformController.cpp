#include "InkStrokeTransformController.h"

void InkStrokeTransformController::Set(double scaleX, double scaleY, double offsetX, double offsetY) {
    scaleX_ = scaleX;
    scaleY_ = scaleY;
    offsetX_ = offsetX;
    offsetY_ = offsetY;
}

VectorStroke InkStrokeTransformController::Apply(const VectorStroke& stroke) const {
    return InkStrokeTransform::Transformed(stroke, scaleX_, scaleY_, offsetX_, offsetY_);
}
