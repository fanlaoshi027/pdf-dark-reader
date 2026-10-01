#pragma once

#include "InkRenderPoint.h"
#include "InkStrokeCap.h"
#include "InkStrokeJoin.h"
#include "InkStrokeTessellator.h"
#include <vector>

class InkVectorGeometry {
public:
    static std::vector<InkTriangle> Build(const std::vector<InkRenderPoint>& input);
};
