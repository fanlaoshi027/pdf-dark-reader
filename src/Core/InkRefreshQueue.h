#pragma once

#include <windows.h>

// Small invalidation helper for low-latency ink rendering.
// Keeps ink repaint requests independent from PDF repaint requests.
class InkRefreshQueue {
public:
    void Reset() noexcept {
        dirty_ = false;
        rect_ = RECT{0, 0, 0, 0};
    }

    void Mark(const RECT& rect) noexcept {
        if (!dirty_) {
            rect_ = rect;
            dirty_ = true;
            return;
        }

        if (rect.left < rect_ .left) rect_.left = rect.left;
        if (rect.top < rect_ .top) rect_.top = rect.top;
        if (rect.right > rect_.right) rect_.right = rect.right;
        if (rect.bottom > rect_.bottom) rect_.bottom = rect.bottom;
    }

    bool HasDirtyRegion() const noexcept { return dirty_; }
    RECT Region() const noexcept { return rect_; }

private:
    bool dirty_ = false;
    RECT rect_{0, 0, 0, 0};
};
