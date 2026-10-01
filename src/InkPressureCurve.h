#pragma once

class InkPressureCurve {
public:
    float Apply(float pressure) const;
    void SetGamma(float gamma);
private:
    float gamma_ = 1.0f;
};
