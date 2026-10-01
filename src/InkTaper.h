#pragma once

#include "InkSample.h"
#include <vector>

class InkTaper {
public:
    static std::vector<InkSample> Apply(const std::vector<InkSample>& input,
                                        double startDistance = 8.0,
                                        double endDistance = 10.0);
};
