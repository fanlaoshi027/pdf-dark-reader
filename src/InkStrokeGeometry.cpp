#include "InkStrokeGeometry.h"

#include <algorithm>
#include <cmath>

namespace {
float PressureToWidth(float pressure, float baseWidth) {
    const float p = std::clamp(pressure, 0.0f, 1.0f);
    const float curved = std::sqrt(p);
    return std::max(0.25f, baseWidth * (0.35f + 0.90f * curved));
}
}

std::vector<InkRenderPoint> InkStrokeGeometry::Build(const VectorStroke& stroke) {
    std::vector<InkRenderPoint> result;
    result.reserve(stroke.points.size());

    for (const auto& point : stroke.points) {
        result.push_back({
            point.x,
            point.y,
            PressureToWidth(point.pressure, stroke.baseWidth)
        });
    }

    return result;
}
