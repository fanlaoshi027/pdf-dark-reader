#include "InkStrokeTransform.h"

VectorStroke InkStrokeTransform::Transformed(const VectorStroke& source, double scaleX, double scaleY, double offsetX, double offsetY) {
    VectorStroke result = source;
    for (auto& point : result.points) {
        point.x = point.x * scaleX + offsetX;
        point.y = point.y * scaleY + offsetY;
    }
    result.width = static_cast<float>(result.width * ((scaleX + scaleY) * 0.5));
    return result;
}
