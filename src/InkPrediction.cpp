#include "InkPrediction.h"

InkSample InkPrediction::Predict(const InkSample& current, const InkSample& previous, const InkLatencyProfile& profile) const {
    InkSample result = current;
    const double dt = current.timestamp - previous.timestamp;
    if (dt <= 0.0) return result;
    const double vx = (current.x - previous.x) / dt;
    const double vy = (current.y - previous.y) / dt;
    const double lead = profile.PredictionSeconds();
    result.x += vx * lead;
    result.y += vy * lead;
    return result;
}
