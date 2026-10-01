#include "InkPointerCoordinator.h"

void InkPointerCoordinator::Begin(HWND hwnd, UINT32 pointerId, std::uint32_t color, float width, bool dashed) {
    hwnd_ = hwnd;
    pointerId_ = pointerId;
    ink_.Begin(color, width, dashed);
}

void InkPointerCoordinator::Update(HWND hwnd, UINT32 pointerId) {
    if (hwnd != hwnd_ || pointerId != pointerId_) return;
    const auto sample = InkPointerSampleExtractor::FromPointerUpdate(hwnd_, pointerId_);
    ink_.Append(sample);
}

bool InkPointerCoordinator::End(UINT32 pointerId) {
    if (pointerId != pointerId_) return false;
    ink_.Finish();
    pointerId_ = 0;
    return true;
}

void InkPointerCoordinator::Cancel() {
    ink_.Cancel();
    pointerId_ = 0;
}
