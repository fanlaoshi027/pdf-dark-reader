#pragma once
#include "InkStrokeHistory.h"
#include "InkStrokeStore.h"

class InkStrokeHistoryController {
public:
    void RecordAdd(InkStrokeStore& store, std::size_t index);
    void RecordRemove(InkStrokeStore& store, std::size_t index);
    bool Undo(InkStrokeStore& store);
    bool Redo(InkStrokeStore& store);
    void Clear();
private:
    InkStrokeHistory history_;
};
