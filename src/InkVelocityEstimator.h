#pragma once
#include "InkSample.h"

struct InkVelocity {
    double x = 0.0;
    double y = 0.0;
    double speed = 0.0;
};

class InkVelocityEstimator {
public:
    InkVelocity Estimate(const InkSample& current, const InkSample& previous) const;
};
