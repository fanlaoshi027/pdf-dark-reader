#pragma once
#include "InkSample.h"
#include "InkLatencyProfile.h"

class InkPrediction {
public:
    InkSample Predict(const InkSample& current, const InkSample& previous, const InkLatencyProfile& profile) const;
};
