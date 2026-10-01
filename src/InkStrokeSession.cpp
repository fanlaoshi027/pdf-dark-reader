#include "InkStrokeSession.h"

void InkStrokeSession::Begin(std::uint32_t color, float width, bool dashed) {
    color_ = color;
    width_ = width;
    dashed_ = dashed;
    samples_.clear();
    active_ = true;
}

void InkStrokeSession::Add(const InkSample& sample) {
    if (!active_) return;
    samples_.push_back(sample);
}

VectorStroke InkStrokeSession::End() {
    if (!active_) return {};
    active_ = false;
    auto result = pipeline_.BuildStroke(samples_, color_, width_, dashed_);
    samples_.clear();
    return result;
}

void InkStrokeSession::Cancel() {
    active_ = false;
    samples_.clear();
}
