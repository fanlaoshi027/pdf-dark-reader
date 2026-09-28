#pragma once

#include <cstdint>

struct InvertSettings {
    bool enabled = false;
    float strength = 0.90f;
    std::uint8_t backgroundR = 26;
    std::uint8_t backgroundG = 26;
    std::uint8_t backgroundB = 26;
};

std::uint32_t InvertPixel(std::uint32_t argb, const InvertSettings& settings);
