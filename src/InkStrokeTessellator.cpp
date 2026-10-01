#include "InkStrokeTessellator.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kMinLengthSquared = 1e-8;

InkVertex OffsetPoint(const InkRenderPoint& point, double nx, double ny) {
    return {point.x + nx * point.width * 0.5,
            point.y + ny * point.width * 0.5};
}

}

std::vector<InkTriangle> InkStrokeTessellator::Build(const std::vector<InkRenderPoint>& points) {
    std::vector<InkTriangle> triangles;
    if (points.size() < 2) return triangles;

    triangles.reserve((points.size() - 1) * 2);

    for (std::size_t i = 0; i + 1 < points.size(); ++i) {
        const auto& p0 = points[i];
        const auto& p1 = points[i + 1];
        const double dx = p1.x - p0.x;
        const double dy = p1.y - p0.y;
        const double lengthSquared = dx * dx + dy * dy;
        if (lengthSquared <= kMinLengthSquared) continue;

        const double invLength = 1.0 / std::sqrt(lengthSquared);
        const double nx = -dy * invLength;
        const double ny = dx * invLength;

        const InkVertex l0 = OffsetPoint(p0, nx, ny);
        const InkVertex r0 = OffsetPoint(p0, -nx, -ny);
        const InkVertex l1 = OffsetPoint(p1, nx, ny);
        const InkVertex r1 = OffsetPoint(p1, -nx, -ny);

        triangles.push_back({l0, r0, l1});
        triangles.push_back({r0, r1, l1});
    }

    return triangles;
}
