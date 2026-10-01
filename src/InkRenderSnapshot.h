#pragma once

#include "VectorStroke.h"
#include <vector>

struct InkRenderSnapshot {
    std::vector<VectorStroke> strokes;
    std::size_t activeStrokePointCount = 0;
};
