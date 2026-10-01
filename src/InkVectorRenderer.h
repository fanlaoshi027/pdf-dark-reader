#pragma once

#include "InkStrokeTessellator.h"
#include <vector>

class InkVectorRenderer {
public:
    virtual ~InkVectorRenderer() = default;
    virtual void Render(const std::vector<InkTriangle>& geometry) = 0;
};
