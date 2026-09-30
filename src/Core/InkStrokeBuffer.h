#pragma once

#include "InkTypes.h"
#include <vector>
#include <chrono>

// Incremental ink buffer used for low-latency pen rendering.
// Keeps the active stroke separate from committed document strokes.
class InkStrokeBuffer {
public:
    void Begin(const InkStroke& stroke) {
        active_ = stroke;
        drawing_ = true;
        dirty_ = true;
    }

    void Append(const InkPoint& point) {
        if (!drawing_) return;
        active_.points.push_back(point);
        dirty_ = true;
    }

    void End() {
        drawing_ = false;
        dirty_ = true;
    }

    void Clear() {
        active_.points.clear();
        drawing_ = false;
        dirty_ = false;
    }

    bool IsDrawing() const { return drawing_; }
    bool IsDirty() const { return dirty_; }

    void MarkRendered() { dirty_ = false; }

    const InkStroke& Current() const { return active_; }

    size_t PointCount() const {
        return active_.points.size();
    }

private:
    InkStroke active_;
    bool drawing_ = false;
    bool dirty_ = false;
};
