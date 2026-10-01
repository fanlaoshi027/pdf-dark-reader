#pragma once

#include "InkSample.h"
#include "InkStrokeBuilder.h"
#include "WindowsInkHistoryAdapter.h"
#include <windows.h>
#include <vector>

class WindowsInkVectorBridge {
public:
    bool Begin(UINT32 pointerId, POINT screenPoint, double originX, double originY, double scale, std::uint32_t color, float baseWidth);
    bool Update(UINT32 pointerId, double originX, double originY, double scale);
    bool End(UINT32 pointerId);
    const VectorStroke& Stroke() const { return stroke_; }
    bool Active() const { return active_; }
    void Reset();

private:
    void Append(const InkSample& sample, double originX, double originY, double scale);

    UINT32 pointerId_ = 0;
    bool active_ = false;
    VectorStroke stroke_;
    std::vector<InkSample> samples_;
    WindowsInkHistoryAdapter history_;
};
