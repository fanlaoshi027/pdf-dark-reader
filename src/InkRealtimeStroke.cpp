#include "InkRealtimeStroke.h"

void InkRealtimeStroke::Begin(std::uint32_t color, float width, bool dashed) {
    color_ = color;
    width_ = width;
    dashed_ = dashed;
    buffer_.Begin();
}

void InkRealtimeStroke::Append(const InkSample& sample) {
    buffer_.Append(sample);
}

VectorStroke InkRealtimeStroke::Preview() const {
    if (!buffer_.Active() || buffer_.Samples().empty()) return {};
    return pipeline_.BuildStroke(buffer_.Samples(), color_, width_, dashed_);
}

VectorStroke InkRealtimeStroke::Finish() {
    if (!buffer_.Active()) return {};
    buffer_.End();
    if (buffer_.Samples().empty()) return {};
    auto result = pipeline_.BuildStroke(buffer_.Samples(), color_, width_, dashed_);
    buffer_.Clear();
    return result;
}

void InkRealtimeStroke::Cancel() {
    buffer_.Clear();
}
