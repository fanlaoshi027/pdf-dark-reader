#pragma once
#include "InkRenderPoint.h"
#include "InkStrokeGeometry.h"
#include <vector>

class InkIncrementalGeometry {
public:
    void Reset();
    bool Append(const InkRenderPoint& point);
    const InkStrokeGeometry& Geometry() const { return geometry_; }
    std::size_t PointCount() const { return points_.size(); }
private:
    std::vector<InkRenderPoint> points_;
    InkStrokeGeometry geometry_;
};
