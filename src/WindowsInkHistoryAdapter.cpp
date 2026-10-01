#include "WindowsInkHistoryAdapter.h"
#include <algorithm>

bool WindowsInkHistoryAdapter::Read(UINT32 pointerId, std::vector<InkSample>& samples) const {
    UINT32 count = 0;
    if (!GetPointerPenInfoHistory(pointerId, &count, nullptr) || count == 0) return false;
    std::vector<POINTER_PEN_INFO> history(count);
    if (!GetPointerPenInfoHistory(pointerId, &count, history.data())) return false;
    samples.clear();
    samples.reserve(count);
    for (const auto& info : history) {
        InkSample sample{};
        sample.x = static_cast<double>(info.pointerInfo.ptPixelLocation.x);
        sample.y = static_cast<double>(info.pointerInfo.ptPixelLocation.y);
        sample.pressure = static_cast<double>(info.pressure) / 1024.0;
        sample.timestamp = static_cast<double>(info.pointerInfo.dwTime) / 1000.0;
        samples.push_back(sample);
    }
    return !samples.empty();
}
