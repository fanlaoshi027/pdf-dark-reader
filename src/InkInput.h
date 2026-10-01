#pragma once

#include "InkSampleBuffer.h"
#include <windows.h>

class InkInput {
public:
    bool Begin(UINT32 pointerId, InkSampleBuffer& buffer);
    bool Update(UINT32 pointerId, InkSampleBuffer& buffer, const InkSample& sample);
    bool End(UINT32 pointerId, InkSampleBuffer& buffer);
    void Cancel();

    bool IsActive() const { return active_; }
    UINT32 PointerId() const { return pointerId_; }

private:
    bool active_ = false;
    UINT32 pointerId_ = 0;
};
