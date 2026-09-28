#include "InvertSettings.h"

#include <algorithm>

namespace {
std::uint8_t ClampByte(float value) {
    return static_cast<std::uint8_t>(std::clamp(value, 0.0f, 255.0f));
}
}

std::uint32_t InvertPixel(std::uint32_t argb, const InvertSettings& settings) {
    if (!settings.enabled) return argb;

    const float s = std::clamp(settings.strength, 0.0f, 1.0f);
    const auto a = static_cast<std::uint8_t>((argb >> 24) & 0xff);
    const auto r = static_cast<std::uint8_t>((argb >> 16) & 0xff);
    const auto g = static_cast<std::uint8_t>((argb >> 8) & 0xff);
    const auto b = static_cast<std::uint8_t>(argb & 0xff);

    // Pure inversion first. Then compress the inverted white point toward
    // the configurable dark background. This keeps the effect an inversion
    // rather than turning it into a generic tint/filter.
    const float invR = 255.0f - r;
    const float invG = 255.0f - g;
    const float invB = 255.0f - b;

    const float targetR = static_cast<float>(settings.backgroundR);
    const float targetG = static_cast<float>(settings.backgroundG);
    const float targetB = static_cast<float>(settings.backgroundB);

    const float outR = targetR + (invR - targetR) * s;
    const float outG = targetG + (invG - targetG) * s;
    const float outB = targetB + (invB - targetB) * s;

    return (static_cast<std::uint32_t>(a) << 24) |
           (static_cast<std::uint32_t>(ClampByte(outR)) << 16) |
           (static_cast<std::uint32_t>(ClampByte(outG)) << 8) |
           static_cast<std::uint32_t>(ClampByte(outB));
}
