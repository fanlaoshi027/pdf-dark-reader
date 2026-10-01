#pragma once

#include "InkSample.h"
#include "VectorStroke.h"
#include <vector>

class InkStrokeBuilder {
public:
    static VectorStroke Build(const std::vector<InkSample>& samples,
                              std::uint32_t color,
                              float baseWidth,
                              bool dashed);
};
