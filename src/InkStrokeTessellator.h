#pragma once

#include "InkStrokeGeometry.h"
#include <vector>

struct InkVertex {
    double x = 0.0;
    double y = 0.0;
};

struct InkTriangle {
    InkVertex a;
    InkVertex b;
    InkVertex c;
};

class InkStrokeTessellator {
public:
    static std::vector<InkTriangle> Build(const std::vector<InkRenderPoint>& points);
};
