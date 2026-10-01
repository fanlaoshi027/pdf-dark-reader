#pragma once
#include "InkSample.h"
#include "InkLatencyProfile.h"

class InkPredictionFilter {
public:
    InkSample Process(const InkSample& sample, const InkLatencyProfile& profile);
    void Reset();
private:
    InkSample last_{};
    bool hasLast_ = false;
};
