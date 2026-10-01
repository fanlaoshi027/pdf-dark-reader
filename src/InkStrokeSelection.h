#pragma once
#include "InkStrokeStore.h"
#include "InkStrokeHitTest.h"

class InkStrokeSelection {
public:
    void Clear();
    bool SelectAt(const InkStrokeStore& store, double x, double y, double tolerance);
    bool HasSelection() const { return index_ != npos; }
    std::size_t Index() const { return index_; }
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);
private:
    std::size_t index_ = npos;
    InkStrokeHitTest hitTest_;
};
