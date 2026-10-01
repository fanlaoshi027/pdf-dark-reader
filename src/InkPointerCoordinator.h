#pragma once
#include "InkRealtimeCoordinator.h"
#include "InkPointerSampleExtractor.h"

class InkPointerCoordinator {
public:
    void Begin(HWND hwnd, UINT32 pointerId, std::uint32_t color, float width, bool dashed);
    void Update(HWND hwnd, UINT32 pointerId);
    bool End(UINT32 pointerId);
    void Cancel();
    InkRealtimeCoordinator& Ink() { return ink_; }
private:
    InkRealtimeCoordinator ink_;
    HWND hwnd_ = nullptr;
    UINT32 pointerId_ = 0;
};
