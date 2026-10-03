#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include "InvertSettings.h"

inline void ApplyPdfInvert(std::vector<std::uint8_t>& pixels, const InvertSettings& s) {
    if (!s.enabled) return;
    const float amount = std::clamp(s.strength, 0.0f, 1.0f);
    for (std::size_t i = 0; i + 3 < pixels.size(); i += 4) {
        const float b = pixels[i] / 255.0f;
        const float g = pixels[i + 1] / 255.0f;
        const float r = pixels[i + 2] / 255.0f;
        const float ir = r + (1.0f - 2.0f * r) * amount;
        const float ig = g + (1.0f - 2.0f * g) * amount;
        const float ib = b + (1.0f - 2.0f * b) * amount;
        const float orr = -0.574f * ir + 1.430f * ig + 0.144f * ib;
        const float org =  0.426f * ir + 0.430f * ig + 0.144f * ib;
        const float orb =  0.426f * ir + 1.430f * ig - 0.856f * ib;
        pixels[i] = static_cast<std::uint8_t>(std::lround(std::clamp(orb, 0.0f, 1.0f) * 255.0f));
        pixels[i + 1] = static_cast<std::uint8_t>(std::lround(std::clamp(org, 0.0f, 1.0f) * 255.0f));
        pixels[i + 2] = static_cast<std::uint8_t>(std::lround(std::clamp(orr, 0.0f, 1.0f) * 255.0f));
    }
}
