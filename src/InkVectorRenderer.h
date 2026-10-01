#pragma once

#include "InkVectorGeometry.h"

class InkVectorRenderer {
public:
    virtual ~InkVectorRenderer() = default;
    virtual void Render(const InkStrokeGeometry& geometry) = 0;
};
