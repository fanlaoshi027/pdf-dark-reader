#include "WindowsInkVectorBridge.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr double kDuplicateDistancePx = 0.05;

bool IsSameSample(const InkSample& a, const InkSample& b) {
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    if (dx * dx + dy * dy > kDuplicateDistancePx * kDuplicateDistancePx) return false;
    if (a.timestamp != 0.0 && b.timestamp != 0.0 && a.timestamp != b.timestamp) return false;
    return true;
}
}

bool WindowsInkVectorBridge::Begin(HWND overlay, UINT32 pointerId, POINT screenPoint, float pressure, double originX, double originY, double scale, std::uint32_t color, float baseWidth) {
    Reset();
    if (!overlay) return false;

    overlay_ = overlay;
    pointerId_ = pointerId;
    active_ = true;
    stroke_.color = color;
    stroke_.baseWidth = baseWidth;

    POINT clientPoint = screenPoint;
    if (!ScreenToClient(overlay_, &clientPoint)) {
        Reset();
        return false;
    }

    InkSample sample{};
    sample.x = static_cast<double>(clientPoint.x);
    sample.y = static_cast<double>(clientPoint.y);
    sample.pressure = pressure > 0.001f ? std::clamp(pressure, 0.0f, 1.0f) : 0.5f;
    samples_.push_back(sample);
    Append(sample, originX, originY, scale);
    return true;
}

bool WindowsInkVectorBridge::Update(UINT32 pointerId, double originX, double originY, double scale) {
    if (!active_ || pointerId != pointerId_ || !overlay_) return false;

    UINT32 count = 0;
    if (!GetPointerPenInfoHistory(pointerId_, &count, nullptr) || count == 0) return false;

    std::vector<POINTER_PEN_INFO> history(count);
    if (!GetPointerPenInfoHistory(pointerId_, &count, history.data()) || count == 0) return false;

    // Windows may return history newest-first. Sort by packet time so the
    // vector stroke always follows the actual pen path instead of connecting
    // packets in reverse order or jumping between duplicated history windows.
    std::stable_sort(history.begin(), history.end(), [](const POINTER_PEN_INFO& a, const POINTER_PEN_INFO& b) {
        return a.pointerInfo.dwTime < b.pointerInfo.dwTime;
    });

    bool appended = false;
    for (const auto& info : history) {
        InkSample sample{};
        sample.x = static_cast<double>(info.pointerInfo.ptPixelLocation.x);
        sample.y = static_cast<double>(info.pointerInfo.ptPixelLocation.y);
        sample.pressure = info.pressure > 0
            ? std::clamp(static_cast<float>(info.pressure) / 1024.0f, 0.0f, 1.0f)
            : (samples_.empty() ? 0.5f : samples_.back().pressure);
        sample.timestamp = static_cast<double>(info.pointerInfo.dwTime) / 1000.0;

        POINT clientPoint{
            static_cast<LONG>(std::lround(sample.x)),
            static_cast<LONG>(std::lround(sample.y))
        };
        if (!ScreenToClient(overlay_, &clientPoint)) continue;
        sample.x = static_cast<double>(clientPoint.x);
        sample.y = static_cast<double>(clientPoint.y);

        if (!samples_.empty()) {
            const auto& last = samples_.back();
            if (IsSameSample(sample, last)) continue;
            if (sample.timestamp != 0.0 && last.timestamp != 0.0 && sample.timestamp < last.timestamp) continue;
        }

        samples_.push_back(sample);
        Append(sample, originX, originY, scale);
        appended = true;
    }
    return appended;
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
    point.pressure = std::clamp(sample.pressure > 0.001f ? sample.pressure : 0.5f, 0.0f, 1.0f);
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
