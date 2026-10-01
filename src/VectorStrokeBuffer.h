#pragma once

#include "VectorStroke.h"
#include <cstddef>

class VectorStrokeBuffer {
public:
    void Begin(std::uint32_t color, float baseWidth, bool dashed);
    void AddPoint(double x, double y, float pressure, std::uint64_t timestamp);
    void End();
    void Clear();

    bool IsActive() const { return active_; }
    std::size_t Size() const { return strokes_.size(); }
    const std::vector<VectorStroke>& Strokes() const { return strokes_; }
    std::vector<VectorStroke>& Strokes() { return strokes_; }

private:
    bool active_ = false;
    std::vector<VectorStroke> strokes_;
};
