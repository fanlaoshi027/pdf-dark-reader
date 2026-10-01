#pragma once

#include "InkSample.h"
#include <vector>

class InkHistorySampler {
public:
    static void Append(InkSample current, std::vector<InkSample>& destination);
};
