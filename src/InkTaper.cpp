#include "InkTaper.h"
#include <algorithm>
#include <cmath>

namespace {
float SmoothStep(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}
}

std::vector<InkSample> InkTaper::Apply(const std::vector<InkSample>& input,
                                       double startDistance,
                                       double endDistance) {
    if (input.size() < 2) return input;

    std::vector<double> distance(input.size(), 0.0);
    for (std::size_t i = 1; i < input.size(); ++i) {
        const double dx = input[i].x - input[i - 1].x;
        const double dy = input[i].y - input[i - 1].y;
        distance[i] = distance[i - 1] + std::sqrt(dx * dx + dy * dy);
    }

    const double total = distance.back();
    if (total <= 0.0) return input;

    const double start = std::max(0.0, startDistance);
    const double end = std::max(0.0, endDistance);

    std::vector<InkSample> output = input;
    for (std::size_t i = 0; i < output.size(); ++i) {
        float factor = 1.0f;
        if (start > 0.0 && distance[i] < start)
            factor *= SmoothStep(static_cast<float>(distance[i] / start));
        if (end > 0.0 && total - distance[i] < end)
            factor *= SmoothStep(static_cast<float>((total - distance[i]) / end));
        output[i].pressure = std::clamp(output[i].pressure * factor, 0.0f, 1.0f);
    }
    return output;
}
