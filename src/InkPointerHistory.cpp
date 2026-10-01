#include "InkPointerHistory.h"
#include "InkPointerSample.h"

std::vector<InkSample> InkPointerHistory::ReadPenHistory(HWND hwnd, UINT32 pointerId, std::uint64_t timestampBase) {
    std::vector<InkSample> result;
    UINT32 count = 0;
    if (!GetPointerInfoHistory(pointerId, &count, nullptr) || count == 0) return result;

    std::vector<POINTER_PEN_INFO> history(count);
    if (!GetPointerPenInfoHistory(pointerId, &count, history.data())) return result;

    result.reserve(count);
    for (UINT32 i = 0; i < count; ++i) {
        POINT pt = history[i].pointerInfo.ptPixelLocation;
        if (ScreenToClient(hwnd, &pt)) {
            result.push_back(InkPointerSample::FromPointerInfo(
                history[i], pt, timestampBase + i));
        }
    }
    return result;
}
