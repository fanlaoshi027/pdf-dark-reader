#include "VectorStrokeBuffer.h"

void VectorStrokeBuffer::Begin(std::uint32_t color, float baseWidth, bool dashed) {
    strokes_.push_back({});
    auto& stroke = strokes_.back();
    stroke.color = color;
    stroke.baseWidth = baseWidth;
    stroke.dashed = dashed;
    active_ = true;
}

void VectorStrokeBuffer::AddPoint(double x, double y, float pressure, std::uint64_t timestamp) {
    if (!active_ || strokes_.empty()) return;
    auto& points = strokes_.back().points;
    if (!points.empty()) {
        const auto& last = points.back();
        const double dx = x - last.x;
        const double dy = y - last.y;
        if (dx * dx + dy * dy < 0.01) return;
    }
    points.push_back({x, y, pressure, timestamp});
}

void VectorStrokeBuffer::End() {
    active_ = false;
    if (!strokes_.empty() && strokes_.back().points.empty()) strokes_.pop_back();
}

void VectorStrokeBuffer::Clear() {
    active_ = false;
    strokes_.clear();
}
