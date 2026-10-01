#include "InkStrokeHitTest.h"
#include <algorithm>
#include <cmath>

namespace {
double SegmentDistance(double px, double py, double ax, double ay, double bx, double by) {
    const double dx = bx - ax;
    const double dy = by - ay;
    const double len2 = dx * dx + dy * dy;
    if (len2 <= 1e-9) return std::hypot(px - ax, py - ay);
    const double t = std::clamp(((px - ax) * dx + (py - ay) * dy) / len2, 0.0, 1.0);
    return std::hypot(px - (ax + t * dx), py - (ay + t * dy));
}
}

bool InkStrokeHitTest::Contains(const VectorStroke& stroke, double x, double y, double tolerance) const {
    if (stroke.points.empty()) return false;
    const double limit = std::max(0.0, tolerance) + stroke.width * 0.5;
    if (stroke.points.size() == 1) return std::hypot(x - stroke.points.front().x, y - stroke.points.front().y) <= limit;
    for (std::size_t i = 1; i < stroke.points.size(); ++i) {
        const auto& a = stroke.points[i - 1];
        const auto& b = stroke.points[i];
        if (SegmentDistance(x, y, a.x, a.y, b.x, b.y) <= limit) return true;
    }
    return false;
}
