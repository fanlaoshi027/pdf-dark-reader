#include "InkCoordinateTransform.h"

VectorStroke InkCoordinateTransform::Apply(const VectorStroke& stroke) const {
    return InkStrokeTransform::Transformed(stroke, scaleX, scaleY, offsetX, offsetY);
}
