#include "InkInput.h"

bool InkInput::Begin(UINT32 pointerId, InkSampleBuffer& buffer) {
    if (active_) return false;
    pointerId_ = pointerId;
    active_ = true;
    buffer.Clear();
    return true;
}

bool InkInput::Update(UINT32 pointerId, InkSampleBuffer& buffer, const InkSample& sample) {
    if (!active_ || pointerId != pointerId_) return false;
    buffer.Add(sample);
    return true;
}

bool InkInput::End(UINT32 pointerId, InkSampleBuffer& buffer) {
    if (!active_ || pointerId != pointerId_) return false;
    active_ = false;
    pointerId_ = 0;
    return !buffer.Empty();
}

void InkInput::Cancel() {
    active_ = false;
    pointerId_ = 0;
}
