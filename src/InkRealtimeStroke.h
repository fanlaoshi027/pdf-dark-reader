#pragma once

#include "InkRealtimeBuffer.h"
#include "InkPipeline.h"
#include "VectorStroke.h"

class InkRealtimeStroke {
public:
    void Begin(std::uint32_t color, float width, bool dashed);
    void Append(const InkSample& sample);
    VectorStroke Preview() const;
    VectorStroke Finish();
    void Cancel();
    bool Active() const { return buffer_.Active(); }
private:
    InkRealtimeBuffer buffer_;
    InkPipeline pipeline_;
    std::uint32_t color_ = 0;
    float width_ = 1.0f;
    bool dashed_ = false;
};
