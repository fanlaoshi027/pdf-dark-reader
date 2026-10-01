#pragma once

#include "VectorStroke.h"
#include <vector>

struct InkRenderPoint {
    double x = 0.0;
    double y = 0.0;
    float width = 1.0f;
};

class InkStrokeGeometry {
public:
    static std::vector<InkRenderPoint> Build(const VectorStroke& stroke);
};
