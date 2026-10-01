#pragma once
#include "InkSample.h"
#include <windows.h>
#include <vector>

class WindowsInkHistoryReader {
public:
    bool Read(UINT32 pointerId, std::vector<InkSample>& samples) const;
};
