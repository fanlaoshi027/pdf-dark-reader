#pragma once

#include <algorithm>
#include <windows.h>

// Lightweight helpers for low-latency pen input.
// Keeps input tuning independent from PDF rendering.
class InkInputOptimization {
public:
    static float ClampPressure(float value) noexcept {
        return std::clamp(value, 0.0f, 1.0f);
    }

    static RECT ExpandDirtyRect(const RECT& rect, LONG padding = 8) noexcept {
        RECT result = rect;
        result.left -= padding;
        result.top -= padding;
        result.right += padding;
        result.bottom += padding;
        return result;
    }
};
