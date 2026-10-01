#pragma once
#include "VectorStroke.h"

class InkStrokeHitTest {
public:
    bool Contains(const VectorStroke& stroke, double x, double y, double tolerance) const;
};
