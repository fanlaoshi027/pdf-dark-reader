#include "InkRealtimeBuffer.h"

void InkRealtimeBuffer::Begin() {
    samples_.clear();
    active_ = true;
}

void InkRealtimeBuffer::Append(const InkSample& sample) {
    if (!active_) return;
    samples_.push_back(sample);
}

void InkRealtimeBuffer::End() {
    active_ = false;
}

void InkRealtimeBuffer::Clear() {
    samples_.clear();
    active_ = false;
}
