#pragma once
#include "InkStrokeStore.h"
#include "InkStrokeRenderer.h"
#include <windows.h>

class InkStrokeLayer {
public:
    std::size_t Add(VectorStroke stroke);
    void Clear();
    void Render(HDC dc) const;
    const InkStrokeStore& Store() const { return store_; }
private:
    InkStrokeStore store_;
    InkStrokeRenderer renderer_;
};
