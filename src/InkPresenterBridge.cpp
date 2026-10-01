#include "InkPresenterBridge.h"

void InkPresenterBridge::Begin(UINT32 pointerId, std::uint32_t color, float width, bool dashed) {
    router_.Begin(pointerId, color, width, dashed);
}

void InkPresenterBridge::Update(UINT32 pointerId, double x, double y, float pressure, std::uint64_t timestamp) {
    router_.Update(pointerId, x, y, pressure, timestamp);
}

bool InkPresenterBridge::End(UINT32 pointerId) {
    return router_.End(pointerId);
}

void InkPresenterBridge::Cancel() {
    router_.Cancel();
}
