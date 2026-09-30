#pragma once
#include <windows.h>

// Tracks the smallest area that needs repainting.
// Used to keep ink rendering independent from PDF rendering.
class DirtyRect {
public:
    void Reset() {
        dirty_ = false;
        rect_ = {0, 0, 0, 0};
    }

    void Add(const RECT& area) {
        if (!dirty_) {
            rect_ = area;
            dirty_ = true;
            return;
        }
        UnionRect(&rect_, &rect_, &area);
    }

    bool Empty() const { return !dirty_; }

    RECT Rect() const { return rect_; }

private:
    RECT rect_{};
    bool dirty_ = false;
};
