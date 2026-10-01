#pragma once

#include "InkSample.h"
#include "VectorStroke.h"
#include "WindowsInkHistoryAdapter.h"
#include <windows.h>
#include <cstdint>
#include <vector>

class WindowsInkVectorBridge {
public:
    bool Begin(HWND overlay, UINT32 pointerId, POINT screenPoint, float pressure, double originX, double originY, double scale, std::uint32_t color, float baseWidth);
    bool Update(UINT32 pointerId, double originX, double originY, double scale);
    bool End(UINT32 pointerId);
    const VectorStroke& Stroke() const { return stroke_; }
    bool Active() const { return active_; }
    void Reset();

private:
    void Append(const InkSample& sample, double originX, double originY, double scale);

    HWND overlay_ = nullptr;
    UINT32 pointerId_ = 0;
    bool active_ = false;
    VectorStroke stroke_;
    std::vector<InkSample> samples_;
    WindowsInkHistoryAdapter history_;
};
