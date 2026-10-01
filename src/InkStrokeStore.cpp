#include "InkStrokeStore.h"

std::size_t InkStrokeStore::Add(VectorStroke stroke) {
    strokes_.push_back(std::move(stroke));
    return strokes_.size() - 1;
}

void InkStrokeStore::Remove(std::size_t index) {
    if (index >= strokes_.size()) return;
    strokes_.erase(strokes_.begin() + static_cast<std::ptrdiff_t>(index));
}

void InkStrokeStore::Clear() {
    strokes_.clear();
}
