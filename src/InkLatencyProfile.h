#pragma once

class InkLatencyProfile {
public:
    float PredictionSeconds() const { return predictionSeconds_; }
    void SetPredictionSeconds(float value);
    float Smoothing() const { return smoothing_; }
    void SetSmoothing(float value);
private:
    float predictionSeconds_ = 0.012f;
    float smoothing_ = 0.18f;
};
