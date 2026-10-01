#pragma once

#include <cstdint>
#include <vector>

struct VectorInkPoint {
    double x = 0.0;
    double y = 0.0;
    float pressure = 0.5f;
    std::uint64_t timestamp = 0;
};

struct VectorStroke {
    std::vector<VectorInkPoint> points;
    std::uint32_t color = 0xFF234B96;
    float baseWidth = 1.0f;
    bool dashed = false;
};
