#include "InkStrokeLayer.h"

std::size_t InkStrokeLayer::Add(VectorStroke stroke) {
    return store_.Add(std::move(stroke));
}

void InkStrokeLayer::Clear() {
    store_.Clear();
}

void InkStrokeLayer::Render(HDC dc) const {
    for (const auto& stroke : store_.Strokes()) {
        renderer_.Render(dc, stroke);
    }
}
