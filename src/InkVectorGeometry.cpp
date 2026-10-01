#include "InkVectorGeometry.h"

std::vector<InkTriangle> InkVectorGeometry::Build(const std::vector<InkRenderPoint>& input) {
    auto points = input;
    InkStrokeJoin::Apply(points);
    InkStrokeCap::AddRoundStart(points);
    InkStrokeCap::AddRoundEnd(points);
    return InkStrokeTessellator::Build(points);
}
