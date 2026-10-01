#pragma once

#include "InkSample.h"
#include <vector>
#include <cstddef>

class InkRealtimeBuffer {
public:
    void Begin();
    void Append(const InkSample& sample);
    void End();
    void Clear();
    const std::vector<InkSample>& Samples() const { return samples_; }
    bool Active() const { return active_; }
    std::size_t Size() const { return samples_.size(); }
private:
    std::vector<InkSample> samples_;
    bool active_ = false;
};
