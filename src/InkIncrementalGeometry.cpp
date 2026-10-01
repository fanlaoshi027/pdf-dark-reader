#include "InkIncrementalGeometry.h"
#include "InkStrokeTessellator.h"
#include "InkStrokeJoin.h"
#include "InkStrokeCap.h"

void InkIncrementalGeometry::Reset() {
    points_.clear();
    geometry_ = {};
}

bool InkIncrementalGeometry::Append(const InkRenderPoint& point) {
    if (!points_.empty()) {
        const auto& last = points_.back();
        const double dx = point.x - last.x;
        const double dy = point.y - last.y;
        if ((dx * dx + dy * dy) < 0.01) return false;
    }
    points_.push_back(point);
    if (points_.size() < 2) return true;

    auto renderPoints = points_;
    InkStrokeJoin::Apply(renderPoints);
    InkStrokeCap::AddRoundStart(renderPoints);
    InkStrokeCap::AddRoundEnd(renderPoints);
    geometry_ = InkStrokeTessellator::Build(renderPoints);
    return true;
}
