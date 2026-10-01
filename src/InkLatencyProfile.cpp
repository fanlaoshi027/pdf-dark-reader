#include "InkLatencyProfile.h"
#include <algorithm>

void InkLatencyProfile::SetPredictionSeconds(float value) {
    predictionSeconds_ = std::clamp(value, 0.0f, 0.04f);
}

void InkLatencyProfile::SetSmoothing(float value) {
    smoothing_ = std::clamp(value, 0.0f, 0.6f);
}
