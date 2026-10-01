#pragma once
#include "VectorStroke.h"

class InkStrokeTransform {
public:
    static VectorStroke Transformed(const VectorStroke& source, double scaleX, double scaleY, double offsetX, double offsetY);
};
