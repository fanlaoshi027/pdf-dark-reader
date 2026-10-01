#include "InkStrokeCap.h"
#include <algorithm>
#include <cmath>

namespace {
void AddCap(std::vector<InkRenderPoint>& points, bool start) {
    if (points.size() < 2) return;
    const auto& p = start ? points.front() : points.back();
    const auto& q = start ? points[1] : points[points.size() - 2];
    const double dx = p.x - q.x;
    const double dy = p.y - q.y;
    const double length = std::hypot(dx, dy);
    if (length < 1e-6) return;
    const double ux = dx / length;
    const double uy = dy / length;
    const double radius = std::max(0.0, static_cast<double>(p.width) * 0.5);
    InkRenderPoint cap = p;
    cap.x += ux * radius;
    cap.y += uy * radius;
    cap.width = 0.0f;
    if (start) points.insert(points.begin(), cap);
    else points.push_back(cap);
}
}

void InkStrokeCap::AddRoundStart(std::vector<InkRenderPoint>& points) { AddCap(points, true); }
void InkStrokeCap::AddRoundEnd(std::vector<InkRenderPoint>& points) { AddCap(points, false); }
