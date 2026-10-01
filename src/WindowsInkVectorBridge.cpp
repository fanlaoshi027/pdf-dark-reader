#include "WindowsInkVectorBridge.h"
#include <algorithm>

bool WindowsInkVectorBridge::Begin(HWND overlay, UINT32 pointerId, POINT screenPoint, float pressure, double originX, double originY, double scale, std::uint32_t color, float baseWidth) {
    Reset();
    if (!overlay) return false;
    overlay_ = overlay;
    pointerId_ = pointerId;
    active_ = true;
    stroke_.color = color;
    stroke_.baseWidth = baseWidth;
    InkSample sample{};
    sample.x = static_cast<double>(screenPoint.x);
    sample.y = static_cast<double>(screenPoint.y);
    sample.pressure = std::clamp(pressure, 0.0f, 1.0f);
    samples_.push_back(sample);
    Append(sample, originX, originY, scale);
    return true;
}

bool WindowsInkVectorBridge::Update(UINT32 pointerId, double originX, double originY, double scale) {
    if (!active_ || pointerId != pointerId_ || !overlay_) return false;
    std::vector<InkSample> history;
    if (!history_.Read(pointerId_, history) || history.empty()) return false;
    for (auto sample : history) {
        if (!samples_.empty() && sample.timestamp != 0 && samples_.back().timestamp != 0 && sample.timestamp <= samples_.back().timestamp) continue;
        POINT screen{static_cast<LONG>(std::lround(sample.x)), static_cast<LONG>(std::lround(sample.y))};
        ScreenToClient(overlay_, &screen);
        sample.x = static_cast<double>(screen.x);
        sample.y = static_cast<double>(screen.y);
        samples_.push_back(sample);
        Append(sample, originX, originY, scale);
    }
    return true;
}

bool WindowsInkVectorBridge::End(UINT32 pointerId) {
    if (!active_ || pointerId != pointerId_) return false;
    active_ = false;
    return true;
}

void WindowsInkVectorBridge::Append(const InkSample& sample, double originX, double originY, double scale) {
    const double s = scale > 0.0 ? scale : 1.0;
    VectorInkPoint point{};
    point.x = (sample.x - originX) / s;
    point.y = (sample.y - originY) / s;
    point.pressure = std::clamp(sample.pressure, 0.0f, 1.0f);
    point.timestamp = sample.timestamp;
    stroke_.points.push_back(point);
}

void WindowsInkVectorBridge::Reset() {
    overlay_ = nullptr;
    pointerId_ = 0;
    active_ = false;
    stroke_ = {};
    samples_.clear();
}
