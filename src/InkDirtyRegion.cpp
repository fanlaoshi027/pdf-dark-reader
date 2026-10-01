#include "InkDirtyRegion.h"
#include <algorithm>
#include <cmath>

void InkDirtyRegion::Reset() {
    rect_ = {0,0,0,0};
    empty_ = true;
}

void InkDirtyRegion::Include(double x, double y, double radius) {
    const LONG left = static_cast<LONG>(std::floor(x - radius));
    const LONG top = static_cast<LONG>(std::floor(y - radius));
    const LONG right = static_cast<LONG>(std::ceil(x + radius));
    const LONG bottom = static_cast<LONG>(std::ceil(y + radius));
    if (empty_) {
        rect_ = {left, top, right, bottom};
        empty_ = false;
        return;
    }
    rect_.left = std::min(rect_.left, left);
    rect_.top = std::min(rect_.top, top);
    rect_.right = std::max(rect_.right, right);
    rect_.bottom = std::max(rect_.bottom, bottom);
}

RECT InkDirtyRegion::Consume() {
    const RECT result = rect_;
    Reset();
    return result;
}
