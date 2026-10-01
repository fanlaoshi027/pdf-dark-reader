#include "InkLiveStroke.h"

void InkLiveStroke::Begin() {
    geometry_.Reset();
    active_ = true;
    hasPrevious_ = false;
}

bool InkLiveStroke::Push(const InkSample& sample, float baseWidth) {
    if (!active_) return false;
    InkRenderPoint point;
    point.x = sample.x;
    point.y = sample.y;
    point.width = baseWidth * (std::max)(0.05f, sample.pressure);
    if (hasPrevious_) {
        const double dx = sample.x - previous_.x;
        const double dy = sample.y - previous_.y;
        if ((dx * dx + dy * dy) < 0.01) return false;
    }
    previous_ = sample;
    hasPrevious_ = true;
    return geometry_.Append(point);
}

void InkLiveStroke::Finish() {
    active_ = false;
}

void InkLiveStroke::Cancel() {
    active_ = false;
    hasPrevious_ = false;
    geometry_.Reset();
}
