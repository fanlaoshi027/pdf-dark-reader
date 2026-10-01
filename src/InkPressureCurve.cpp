#include "InkPressureCurve.h"
#include <algorithm>
#include <cmath>

float InkPressureCurve::Apply(float pressure) const {
    const float p = std::clamp(pressure, 0.0f, 1.0f);
    return std::pow(p, gamma_);
}

void InkPressureCurve::SetGamma(float gamma) {
    gamma_ = std::clamp(gamma, 0.25f, 4.0f);
}
