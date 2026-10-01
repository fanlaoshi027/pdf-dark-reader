#include "InkStrokeJoin.h"
#include <algorithm>
#include <cmath>

void InkStrokeJoin::Apply(std::vector<InkRenderPoint>& points) {
    if (points.size() < 3) return;
    for (std::size_t i = 1; i + 1 < points.size(); ++i) {
        const auto& a = points[i - 1];
        const auto& b = points[i];
        const auto& c = points[i + 1];
        const double inX = b.x - a.x;
        const double inY = b.y - a.y;
        const double outX = c.x - b.x;
        const double outY = c.y - b.y;
        const double inLen = std::hypot(inX, inY);
        const double outLen = std::hypot(outX, outY);
        if (inLen < 1e-6 || outLen < 1e-6) continue;
        const double turn = std::clamp((inX * outX + inY * outY) / (inLen * outLen), -1.0, 1.0);
        const float limit = static_cast<float>(std::clamp(0.5 + 0.5 * turn, 0.65, 1.0));
        points[i].width *= limit;
    }
}
