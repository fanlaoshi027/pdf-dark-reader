#pragma once

#include <cstdint>

// Display-only theme mapping. The PDF file itself is never modified.
struct InvertSettings {
    bool enabled = false;
    float strength = 1.0f;

    // Dark page background. Default is charcoal.
    std::uint8_t backgroundR = 32;
    std::uint8_t backgroundG = 33;
    std::uint8_t backgroundB = 36;

    // Light foreground used for black/dark source pixels.
    std::uint8_t foregroundR = 224;
    std::uint8_t foregroundG = 224;
    std::uint8_t foregroundB = 224;
};

std::uint32_t InvertPixel(std::uint32_t argb, const InvertSettings& settings);
