#pragma once
#include "VectorStroke.h"
#include <vector>

class InkStrokeStore {
public:
    std::size_t Add(VectorStroke stroke);
    void Remove(std::size_t index);
    void Clear();
    const std::vector<VectorStroke>& Strokes() const { return strokes_; }
private:
    std::vector<VectorStroke> strokes_;
};
