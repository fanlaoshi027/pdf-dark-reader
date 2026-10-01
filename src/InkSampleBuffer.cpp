#include "InkSampleBuffer.h"

void InkSampleBuffer::Clear() {
    samples_.clear();
}

void InkSampleBuffer::Add(const InkSample& sample) {
    samples_.push_back(sample);
}
