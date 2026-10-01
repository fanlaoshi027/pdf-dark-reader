#include "InkInputAdapter.h"

void InkInputAdapter::BeginStroke(std::uint32_t color, float width, bool dashed) {
    session_.Begin(color, width, dashed);
}

void InkInputAdapter::PushSample(double x, double y, float pressure, std::uint64_t timestamp) {
    if (!session_.Active()) return;
    session_.Add({x, y, pressure, timestamp});
}

VectorStroke InkInputAdapter::EndStroke() {
    return session_.End();
}

void InkInputAdapter::CancelStroke() {
    session_.Cancel();
}
