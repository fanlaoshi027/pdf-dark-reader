#include "InvertSettings.h"

#include <algorithm>
#include <cmath>

namespace {
std::uint8_t ClampByte(float value) {
    return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0f, 255.0f)));
}

float MapChannel(float source, float background, float foreground, float strength) {
    // White paper (255) maps to the selected dark background.
    // Black print (0) maps to the light foreground.
    const float mapped = background + (255.0f - source) * (foreground - background) / 255.0f;
    return source + (mapped - source) * strength;
}
}

std::uint32_t InvertPixel(std::uint32_t argb, const InvertSettings& settings) {
    if (!settings.enabled) return argb;

    const float amount = std::clamp(settings.strength, 0.0f, 1.0f);
    const float r = static_cast<float>((argb >> 16) & 0xff);
    const float g = static_cast<float>((argb >> 8) & 0xff);
    const float b = static_cast<float>(argb & 0xff);

    const auto outR = ClampByte(MapChannel(r, settings.backgroundR, settings.foregroundR, amount));
    const auto outG = ClampByte(MapChannel(g, settings.backgroundG, settings.foregroundG, amount));
    const auto outB = ClampByte(MapChannel(b, settings.backgroundB, settings.foregroundB, amount));

    // Preserve alpha exactly.
    const std::uint32_t a = (argb >> 24) & 0xff;
    return (a << 24) |
           (static_cast<std::uint32_t>(outR) << 16) |
           (static_cast<std::uint32_t>(outG) << 8) |
           static_cast<std::uint32_t>(outB);
}
