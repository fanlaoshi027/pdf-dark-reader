#pragma once
#include "InkSample.h"
#include "InkVelocityEstimator.h"

class InkAdaptiveSmoothing {
public:
    InkSample Apply(const InkSample& sample, const InkVelocity& velocity);
    void Reset();
private:
    InkSample output_{};
    bool initialized_ = false;
};
