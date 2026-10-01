#include "InkAdaptiveSmoothing.h"
#include <algorithm>

InkSample InkAdaptiveSmoothing::Apply(const InkSample& sample, const InkVelocity& velocity) {
    if (!initialized_) {
        output_ = sample;
        initialized_ = true;
        return output_;
    }
    const double t = std::clamp(0.04 / (0.04 + velocity.speed), 0.08, 0.40);
    output_.x += (sample.x - output_.x) * t;
    output_.y += (sample.y - output_.y) * t;
    output_.pressure += (sample.pressure - output_.pressure) * t;
    output_.timestamp = sample.timestamp;
    return output_;
}

void InkAdaptiveSmoothing::Reset() {
    initialized_ = false;
    output_ = {};
}
