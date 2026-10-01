#pragma once
#include "InkRealtimeStroke.h"
#include "InkRenderScheduler.h"
#include "InkRenderFrame.h"

class InkRealtimeCoordinator {
public:
    void Begin(std::uint32_t color, float width, bool dashed);
    void Append(const InkSample& sample);
    void Finish();
    void Cancel();
    void RequestRender();
    bool NeedsRender();
    InkRenderFrame BuildFrame() const;
    const InkRealtimeStroke& Stroke() const { return stroke_; }
private:
    InkRealtimeStroke stroke_;
    InkRenderScheduler scheduler_;
};
