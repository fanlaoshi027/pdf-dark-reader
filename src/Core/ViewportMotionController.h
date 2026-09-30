#pragma once

#include <algorithm>

class ViewportMotionController {
public:
    void SetTarget(double zoom) noexcept {
        targetZoom_ = std::clamp(zoom, 0.25, 8.0);
    }

    double Step(double current) noexcept {
        const double delta = targetZoom_ - current;
        if (delta > -0.001 && delta < 0.001) return targetZoom_;
        return current + delta * 0.18;
    }

    double targetZoom() const noexcept { return targetZoom_; }

private:
    double targetZoom_ = 1.0;
};
