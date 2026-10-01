#include "InkRealtimeCoordinator.h"
#include "InkVectorGeometry.h"

void InkRealtimeCoordinator::Begin(std::uint32_t color, float width, bool dashed) {
    stroke_.Begin(color, width, dashed);
    scheduler_.Request();
}

void InkRealtimeCoordinator::Append(const InkSample& sample) {
    if (!stroke_.Active()) return;
    stroke_.Append(sample);
    scheduler_.Request();
}

void InkRealtimeCoordinator::Finish() {
    if (!stroke_.Active()) return;
    stroke_.Finish();
    scheduler_.Request();
}

void InkRealtimeCoordinator::Cancel() {
    stroke_.Cancel();
    scheduler_.Request();
}

void InkRealtimeCoordinator::RequestRender() {
    scheduler_.Request();
}

bool InkRealtimeCoordinator::NeedsRender() {
    return scheduler_.Consume();
}

InkRenderFrame InkRealtimeCoordinator::BuildFrame() const {
    InkRenderFrame frame;
    if (stroke_.Active()) {
        const auto preview = stroke_.Preview();
        if (!preview.points.empty()) {
            frame.preview = InkVectorGeometry::Build({
                preview.points.begin(), preview.points.end()
            });
        }
    }
    return frame;
}
