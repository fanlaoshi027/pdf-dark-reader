#pragma once
#include "InkSample.h"
#include <windows.h>
#include <vector>

class InkPointerHistory {
public:
    static std::vector<InkSample> ReadPenHistory(HWND hwnd, UINT32 pointerId, std::uint64_t timestampBase);
};
