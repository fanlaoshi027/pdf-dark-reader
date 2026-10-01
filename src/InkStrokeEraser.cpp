#include "InkStrokeEraser.h"

bool InkStrokeEraser::EraseAt(InkStrokeStore& store, double x, double y, double radius) {
    const auto& strokes = store.Strokes();
    for (std::size_t i = strokes.size(); i-- > 0;) {
        if (hitTest_.Contains(strokes[i], x, y, radius)) {
            store.Remove(i);
            return true;
        }
    }
    return false;
}
