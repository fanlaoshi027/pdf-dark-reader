#include "InkPredictionFilter.h"
#include "InkPrediction.h"

InkSample InkPredictionFilter::Process(const InkSample& sample, const InkLatencyProfile& profile) {
    if (!hasLast_) {
        last_ = sample;
        hasLast_ = true;
        return sample;
    }
    InkPrediction prediction;
    InkSample result = prediction.Predict(sample, last_, profile);
    last_ = sample;
    return result;
}

void InkPredictionFilter::Reset() {
    hasLast_ = false;
    last_ = {};
}
