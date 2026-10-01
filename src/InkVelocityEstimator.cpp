#include "InkVelocityEstimator.h"
#include <cmath>

InkVelocity InkVelocityEstimator::Estimate(const InkSample& current, const InkSample& previous) const {
    InkVelocity result;
    const double dt = current.timestamp - previous.timestamp;
    if (dt <= 0.0) return result;
    result.x = (current.x - previous.x) / dt;
    result.y = (current.y - previous.y) / dt;
    result.speed = std::hypot(result.x, result.y);
    return result;
}
