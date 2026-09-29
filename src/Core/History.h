#pragma once
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

// Small platform-neutral undo/redo stack. Actions are supplied by the caller,
// so the core does not depend on Win32, Qt, Cocoa, or any UI framework.
class History {
public:
    using Action = std::function<void()>;

    void Clear() { undo_.clear(); redo_.clear(); }
    bool CanUndo() const { return !undo_.empty(); }
    bool CanRedo() const { return !redo_.empty(); }

    void Push(Action undoAction, Action redoAction) {
        if (!undoAction || !redoAction) return;
        undo_.push_back({std::move(undoAction), std::move(redoAction)});
        redo_.clear();
    }

    bool Undo() {
        if (undo_.empty()) return false;
        Entry entry = std::move(undo_.back());
        undo_.pop_back();
        entry.undo();
        redo_.push_back(std::move(entry));
        return true;
    }

    bool Redo() {
        if (redo_.empty()) return false;
        Entry entry = std::move(redo_.back());
        redo_.pop_back();
        entry.redo();
        undo_.push_back(std::move(entry));
        return true;
    }

private:
    struct Entry { Action undo; Action redo; };
    std::vector<Entry> undo_;
    std::vector<Entry> redo_;
};
