#pragma once
#include "VectorStroke.h"
#include <vector>

class InkStrokeHistory {
public:
    void RecordAdd(VectorStroke stroke);
    void RecordRemove(std::size_t index, VectorStroke stroke);
    bool Undo(std::vector<VectorStroke>& strokes);
    bool Redo(std::vector<VectorStroke>& strokes);
    void Clear();
private:
    struct Action { bool added; std::size_t index; VectorStroke stroke; };
    std::vector<Action> undo_;
    std::vector<Action> redo_;
};
