#pragma once

#include "InkSample.h"
#include <vector>

class InkSampleBuffer {
public:
    void Clear();
    void Add(const InkSample& sample);

    const std::vector<InkSample>& Samples() const { return samples_; }
    std::vector<InkSample>& Samples() { return samples_; }
    bool Empty() const { return samples_.empty(); }

private:
    std::vector<InkSample> samples_;
};
