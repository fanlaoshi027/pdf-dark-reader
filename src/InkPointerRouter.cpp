#include "InkPointerRouter.h"
#include "InkPointerHistory.h"

void InkPointerRouter::Begin(UINT32 pointerId, std::uint32_t color, float width, bool dashed) {
    pointerId_ = pointerId;
    input_.BeginStroke(color, width, dashed);
}

void InkPointerRouter::Update(UINT32 pointerId, double x, double y, float pressure, std::uint64_t timestamp) {
    if (!input_.Active() || pointerId != pointerId_) return;
    input_.PushSample(x, y, pressure, timestamp);
}

bool InkPointerRouter::End(UINT32 pointerId) {
    if (!input_.Active() || pointerId != pointerId_) return false;
    auto stroke = input_.EndStroke();
    if (stroke.points.empty()) return false;
    store_.Add(std::move(stroke));
    pointerId_ = 0;
    return true;
}

void InkPointerRouter::Cancel() {
    input_.CancelStroke();
    pointerId_ = 0;
}

void InkPointerRouter::UpdateWithHistory(HWND hwnd, UINT32 pointerId,
                                         double x, double y, float pressure,
                                         std::uint64_t timestamp) {
    if (!input_.Active() || pointerId != pointerId_) return;
    const auto history = InkPointerHistory::ReadPenHistory(hwnd, pointerId, timestamp);
    for (const auto& sample : history) {
        input_.PushSample(sample.x, sample.y, sample.pressure, sample.timestamp);
    }
    input_.PushSample(x, y, pressure, timestamp + history.size());
}
