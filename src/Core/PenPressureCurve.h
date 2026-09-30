#pragma once

#include <algorithm>
#include <cmath>

// Pressure response curve for natural handwriting.
// Keeps light strokes thin and avoids sudden width jumps.
class PenPressureCurve {
public:
    static float Apply(float pressure) {
        pressure = std::clamp(pressure, 0.0f, 1.0f);
        // Smooth ease curve similar to tablet ink response.
        return pressure * pressure * (3.0f - 2.0f * pressure);
    }

    static float Width(float minWidth, float maxWidth, float pressure) {
        const float p = Apply(pressure);
        return minWidth + (maxWidth - minWidth) * p;
    }
};
