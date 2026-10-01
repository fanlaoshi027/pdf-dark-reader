#pragma once

#include "InkSample.h"
#include <vector>

class InkSmoother {
public:
    static std::vector<InkSample> Smooth(const std::vector<InkSample>& input);
};
