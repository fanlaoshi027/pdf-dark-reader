#pragma once

#include "InkRenderPoint.h"
#include "InkStrokeCap.h"
#include "InkStrokeJoin.h"
#include "InkStrokeTessellator.h"

class InkVectorGeometry {
public:
    static InkStrokeGeometry Build(const std::vector<InkRenderPoint>& input) {
        auto points = input;
        InkStrokeJoin::Apply(points);
        InkStrokeCap::AddRoundStart(points);
        InkStrokeCap::AddRoundEnd(points);
        return InkStrokeTessellator::Build(points);
    }
};
