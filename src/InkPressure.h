#pragma once

#include <algorithm>

class InkPressure {
public:
    static float Normalize(float pressure) {
        return std::clamp(pressure, 0.0f, 1.0f);
    }

    static float Apply(float pressure) {
        const float p = Normalize(pressure);
        return std::clamp(p * p * (3.0f - 2.0f * p), 0.0f, 1.0f);
    }
};
