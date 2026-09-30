#pragma once

#include "InkTypes.h"
#include <vector>

// Incremental ink buffer used for low-latency pen rendering.
// Keeps the active stroke separate from committed document strokes.
class InkStrokeBuffer {
public:
    void Begin(const InkStroke& stroke) {
        active_ = stroke;
        drawing_ = true;
    }

    void Append(const InkPoint& point) {
        if (!drawing_) return;
        active_.points.push_back(point);
    }

    void Clear() {
        active_.points.clear();
        drawing_ = false;
    }

    bool IsDrawing() const { return drawing_; }

    const InkStroke& Current() const { return active_; }

private:
    InkStroke active_;
    bool drawing_ = false;
};
