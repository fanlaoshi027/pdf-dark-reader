#include "InkStrokeHistoryController.h"

void InkStrokeHistoryController::RecordAdd(InkStrokeStore& store, std::size_t index) {
    const auto& strokes = store.Strokes();
    if (index < strokes.size()) history_.RecordAdd(strokes[index]);
}

void InkStrokeHistoryController::RecordRemove(InkStrokeStore& store, std::size_t index) {
    const auto& strokes = store.Strokes();
    if (index < strokes.size()) history_.RecordRemove(index, strokes[index]);
}

bool InkStrokeHistoryController::Undo(InkStrokeStore& store) {
    auto& mutableStrokes = const_cast<std::vector<VectorStroke>&>(store.Strokes());
    return history_.Undo(mutableStrokes);
}

bool InkStrokeHistoryController::Redo(InkStrokeStore& store) {
    auto& mutableStrokes = const_cast<std::vector<VectorStroke>&>(store.Strokes());
    return history_.Redo(mutableStrokes);
}

void InkStrokeHistoryController::Clear() {
    history_.Clear();
}
