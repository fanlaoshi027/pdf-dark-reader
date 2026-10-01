#pragma once
#include "InkSample.h"
#include "InkPipeline.h"
#include "VectorStroke.h"
#include <vector>

class InkStrokeSession {
public:
    void Begin(std::uint32_t color, float width, bool dashed);
    void Add(const InkSample& sample);
    VectorStroke End();
    void Cancel();
    bool Active() const { return active_; }
private:
    bool active_ = false;
    std::uint32_t color_ = 0;
    float width_ = 1.0f;
    bool dashed_ = false;
    std::vector<InkSample> samples_;
    InkPipeline pipeline_;
};
