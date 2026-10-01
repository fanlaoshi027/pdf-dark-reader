#include "InkStrokeSelection.h"

void InkStrokeSelection::Clear() {
    index_ = npos;
}

bool InkStrokeSelection::SelectAt(const InkStrokeStore& store, double x, double y, double tolerance) {
    index_ = npos;
    const auto& strokes = store.Strokes();
    for (std::size_t i = strokes.size(); i-- > 0;) {
        if (hitTest_.Contains(strokes[i], x, y, tolerance)) {
            index_ = i;
            return true;
        }
    }
    return false;
}
