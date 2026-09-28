#include "InvertSettings.h"

#include <algorithm>
#include <cmath>

namespace {
std::uint8_t ClampByte(float value) {
    return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0f, 255.0f)));
}
}

std::uint32_t InvertPixel(std::uint32_t argb, const InvertSettings& settings) {
    if (!settings.enabled) return argb;

    const float amount = std::clamp(settings.strength, 0.0f, 1.0f);
    const float r = ((argb >> 16) & 0xff) / 255.0f;
    const float g = ((argb >> 8) & 0xff) / 255.0f;
    const float b = (argb & 0xff) / 255.0f;

    const float ir = r + (1.0f - 2.0f * r) * amount;
    const float ig = g + (1.0f - 2.0f * g) * amount;
    const float ib = b + (1.0f - 2.0f * b) * amount;

    // CSS: invert(amount) hue-rotate(180deg)
    const float outR = -0.574f * ir + 1.430f * ig + 0.144f * ib;
    const float outG =  0.426f * ir + 0.430f * ig + 0.144f * ib;
    const float outB =  0.426f * ir + 1.430f * ig - 0.856f * ib;

    const std::uint32_t a = (argb >> 24) & 0xff;
    return (a << 24) |
           (static_cast<std::uint32_t>(ClampByte(outR * 255.0f)) << 16) |
           (static_cast<std::uint32_t>(ClampByte(outG * 255.0f)) << 8) |
           static_cast<std::uint32_t>(ClampByte(outB * 255.0f));
}
