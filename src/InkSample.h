#pragma once

#include <cstdint>

struct InkSample {
    double x = 0.0;
    double y = 0.0;
    float pressure = 0.5f;
    std::uint64_t timestamp = 0;
};
