#include "InkStrokeHistory.h"
#include <algorithm>

void InkStrokeHistory::RecordAdd(VectorStroke stroke) {
    undo_.push_back({true, undo_.size(), std::move(stroke)});
    redo_.clear();
}

void InkStrokeHistory::RecordRemove(std::size_t index, VectorStroke stroke) {
    undo_.push_back({false, index, std::move(stroke)});
    redo_.clear();
}

bool InkStrokeHistory::Undo(std::vector<VectorStroke>& strokes) {
    if (undo_.empty()) return false;
    Action action = std::move(undo_.back());
    undo_.pop_back();
    if (action.added) {
        if (action.index < strokes.size()) strokes.erase(strokes.begin() + static_cast<std::ptrdiff_t>(action.index));
    } else {
        const auto index = std::min(action.index, strokes.size());
        strokes.insert(strokes.begin() + static_cast<std::ptrdiff_t>(index), action.stroke);
    }
    redo_.push_back(std::move(action));
    return true;
}

bool InkStrokeHistory::Redo(std::vector<VectorStroke>& strokes) {
    if (redo_.empty()) return false;
    Action action = std::move(redo_.back());
    redo_.pop_back();
    if (action.added) {
        const auto index = std::min(action.index, strokes.size());
        strokes.insert(strokes.begin() + static_cast<std::ptrdiff_t>(index), action.stroke);
    } else if (action.index < strokes.size()) {
        strokes.erase(strokes.begin() + static_cast<std::ptrdiff_t>(action.index));
    }
    undo_.push_back(std::move(action));
    return true;
}

void InkStrokeHistory::Clear() {
    undo_.clear();
    redo_.clear();
}
