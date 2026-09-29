#include "StrokeDynamics.h"
#include <algorithm>
#include <cmath>

float StrokeDynamics::ClampPressure(float pressure) {
    return std::clamp(pressure, 0.05f, 1.0f);
}

float StrokeDynamics::WidthFromPressure(float pressure, float baseWidth) {
    const float p = ClampPressure(pressure);
    const float factor = 0.55f + p * 0.90f;
    return std::max(0.5f, baseWidth * factor);
}

float StrokeDynamics::Speed(const InkPoint& a, const InkPoint& b) {
    const double dx = b.pdfX - a.pdfX;
    const double dy = b.pdfY - a.pdfY;
    return static_cast<float>(std::sqrt(dx * dx + dy * dy));
}

void StrokeDynamics::SmoothPoint(InkStroke& stroke, const InkPoint& point) {
    if (stroke.points.empty()) {
        stroke.points.push_back(point);
        return;
    }
    if (stroke.points.size() < 3) {
        stroke.points.push_back(point);
        return;
    }
    const auto& prev = stroke.points[stroke.points.size() - 1];
    const float alpha = 0.72f;
    InkPoint smoothed = point;
    smoothed.pdfX = prev.pdfX + (point.pdfX - prev.pdfX) * alpha;
    smoothed.pdfY = prev.pdfY + (point.pdfY - prev.pdfY) * alpha;
    smoothed.pressure = prev.pressure + (point.pressure - prev.pressure) * alpha;
    stroke.points.push_back(smoothed);
}

void StrokeDynamics::ApplyTaper(InkStroke& stroke, float startRatio, float endRatio) {
    const size_t n = stroke.points.size();
    if (n < 3) return;
    startRatio = std::clamp(startRatio, 0.0f, 0.45f);
    endRatio = std::clamp(endRatio, 0.0f, 0.45f);
    for (size_t i = 0; i < n; ++i) {
        float factor = 1.0f;
        const float start = static_cast<float>(i) / static_cast<float>(n - 1);
        const float end = static_cast<float>(n - 1 - i) / static_cast<float>(n - 1);
        if (startRatio > 0.0f && start < startRatio) factor *= start / startRatio;
        if (endRatio > 0.0f && end < endRatio) factor *= end / endRatio;
        stroke.points[i].pressure = std::max(0.05f, stroke.points[i].pressure * factor);
    }
}
