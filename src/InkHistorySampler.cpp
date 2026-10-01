#include "InkHistorySampler.h"
#include <cmath>

void InkHistorySampler::Append(InkSample current, std::vector<InkSample>& destination) {
    if (!destination.empty()) {
        const auto& last = destination.back();
        const double dx = current.x - last.x;
        const double dy = current.y - last.y;
        if (dx * dx + dy * dy < 0.0001 && current.timestamp == last.timestamp) return;
    }
    destination.push_back(current);
}
