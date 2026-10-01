#include "DocumentViewportAdapter.h"

void DocumentViewportAdapter::SyncFromTransform() {
    const auto& state = transform_.State();
    viewport_.SetZoom(state.scale);
    viewport_.SetOffset(state.offsetX, state.offsetY);
}

void DocumentViewportAdapter::SyncToTransform() {
    const auto offset = viewport_.Offset();
    transform_.SetScale(viewport_.Zoom());
    transform_.State().offsetX = offset.x;
    transform_.State().offsetY = offset.y;
}
