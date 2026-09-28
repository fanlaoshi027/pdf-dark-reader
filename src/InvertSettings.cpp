#include "InvertSettings.h"

#include <algorithm>
#include <cmath>

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

    // Important for teaching PDFs: do NOT invert chromatic colors.
    // Red stays red, blue stays blue, etc. Only neutral/gray pixels are
    // inverted. This gives the desired black-text-on-dark-paper effect
    // without turning colored annotations into their complementary colors.
    const int maxChannel = std::max({ static_cast<int>(r), static_cast<int>(g), static_cast<int>(b) });
    const int minChannel = std::min({ static_cast<int>(r), static_cast<int>(g), static_cast<int>(b) });
    constexpr int kNeutralTolerance = 18;

    if (maxChannel - minChannel > kNeutralTolerance) {
        return argb; // Preserve actual color.
    }

    // Neutral grayscale: white -> configurable dark background,
    // black -> light foreground, with the same strength control.
    const float gray = (static_cast<float>(r) + g + b) / 3.0f;
    const float inverted = 255.0f - gray;

    const float targetR = static_cast<float>(settings.backgroundR);
    const float targetG = static_cast<float>(settings.backgroundG);
    const float targetB = static_cast<float>(settings.backgroundB);

    const float outR = targetR + (inverted - targetR) * s;
    const float outG = targetG + (inverted - targetG) * s;
    const float outB = targetB + (inverted - targetB) * s;

    return (static_cast<std::uint32_t>(a) << 24) |
           (static_cast<std::uint32_t>(ClampByte(outR)) << 16) |
           (static_cast<std::uint32_t>(ClampByte(outG)) << 8) |
           static_cast<std::uint32_t>(ClampByte(outB));
}
