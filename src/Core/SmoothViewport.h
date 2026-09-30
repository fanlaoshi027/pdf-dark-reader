#pragma once
#include <algorithm>

// Smooth PDF viewport controller.
// Keeps zoom/pan changes incremental instead of forcing abrupt jumps.
class SmoothViewport {
public:
    void SetTargetZoom(double zoom) {
        targetZoom_ = std::clamp(zoom, 0.25, 8.0);
    }

    void Step() {
        zoom_ += (targetZoom_ - zoom_) * 0.18;
        if (std::abs(targetZoom_ - zoom_) < 0.001) zoom_ = targetZoom_;
    }

    double Zoom() const { return zoom_; }
    double TargetZoom() const { return targetZoom_; }

private:
    double zoom_ = 1.0;
    double targetZoom_ = 1.0;
};
