#pragma once

#include "InkRenderPoint.h"
#include "VectorStroke.h"
#include <vector>

class InkStrokeGeometry {
public:
    static std::vector<InkRenderPoint> Build(const VectorStroke& stroke);
};
