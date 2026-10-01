#pragma once

#include "InkRenderPoint.h"
#include <vector>

class InkStrokeJoin {
public:
    static void Apply(std::vector<InkRenderPoint>& points);
};
