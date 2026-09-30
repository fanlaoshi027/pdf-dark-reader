#pragma once

#include <windows.h>

class LocalRefreshRegion {
public:
    void Reset() noexcept {
        dirty_ = false;
        rect_ = RECT{0, 0, 0, 0};
    }

    void Add(const RECT& rect) noexcept {
        if (!dirty_) {
            rect_ = rect;
            dirty_ = true;
            return;
        }
        UnionRect(&rect_, &rect_, &rect);
    }

    bool Dirty() const noexcept { return dirty_; }
    const RECT& Rect() const noexcept { return rect_; }

private:
    RECT rect_{0, 0, 0, 0};
    bool dirty_ = false;
};
