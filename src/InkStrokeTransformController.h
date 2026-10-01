#pragma once
#include "InkStrokeTransform.h"

class InkStrokeTransformController {
public:
    void Set(double scaleX, double scaleY, double offsetX, double offsetY);
    VectorStroke Apply(const VectorStroke& stroke) const;
private:
    double scaleX_ = 1.0;
    double scaleY_ = 1.0;
    double offsetX_ = 0.0;
    double offsetY_ = 0.0;
};
