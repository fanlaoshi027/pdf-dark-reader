#pragma once

#include <windows.h>
#include <algorithm>

struct InkDirtyRegion {
    RECT rect{};
    bool dirty = false;

    void Clear() noexcept {
        dirty = false;
        rect = RECT{};
    }

    void Include(const RECT& value) noexcept {
        if (!dirty) {
            rect = value;
            dirty = true;
            return;
        }

        rect.left = std::min(rect.left, value.left);
        rect.top = std::min(rect.top, value.top);
        rect.right = std::max(rect.right, value.right);
        rect.bottom = std::max(rect.bottom, value.bottom);
    }

    void IncludePoint(POINT point, int padding = 8) noexcept {
        Include(RECT{point.x - padding, point.y - padding, point.x + padding, point.y + padding});
    }
};
