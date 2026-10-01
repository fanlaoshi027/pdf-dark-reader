#pragma once
#include "InkCoordinateSpace.h"
#include "InkStrokeTransform.h"

struct InkCoordinateTransform {
    InkCoordinateSpace from = InkCoordinateSpace::Page;
    InkCoordinateSpace to = InkCoordinateSpace::View;
    double scaleX = 1.0;
    double scaleY = 1.0;
    double offsetX = 0.0;
    double offsetY = 0.0;

    VectorStroke Apply(const VectorStroke& stroke) const;
};
