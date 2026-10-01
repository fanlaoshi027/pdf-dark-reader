#pragma once

#include "InkSample.h"
#include "VectorStroke.h"
#include <cstdint>
#include <vector>

class InkPipeline {
public:
    VectorStroke BuildStroke(const std::vector<InkSample>& samples,
                             std::uint32_t color,
                             float baseWidth,
                             bool dashed) const;
};
