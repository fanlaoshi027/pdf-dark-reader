#include "InkSmoother.h"
#include <algorithm>

std::vector<InkSample> InkSmoother::Smooth(const std::vector<InkSample>& input) {
    if (input.size() < 3) return input;

    std::vector<InkSample> output;
    output.reserve(input.size());
    output.push_back(input.front());

    for (std::size_t i = 1; i + 1 < input.size(); ++i) {
        const auto& a = input[i - 1];
        const auto& b = input[i];
        const auto& c = input[i + 1];
        InkSample s = b;
        s.x = (a.x + 2.0 * b.x + c.x) * 0.25;
        s.y = (a.y + 2.0 * b.y + c.y) * 0.25;
        s.pressure = std::clamp((a.pressure + 2.0f * b.pressure + c.pressure) * 0.25f, 0.0f, 1.0f);
        output.push_back(s);
    }

    output.push_back(input.back());
    return output;
}
