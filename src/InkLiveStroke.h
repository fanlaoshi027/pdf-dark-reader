#pragma once
#include "InkIncrementalGeometry.h"
#include "InkSample.h"

class InkLiveStroke {
public:
    void Begin();
    bool Push(const InkSample& sample, float baseWidth);
    void Finish();
    void Cancel();
    bool Active() const { return active_; }
    const InkIncrementalGeometry& Geometry() const { return geometry_; }
private:
    bool active_ = false;
    InkIncrementalGeometry geometry_;
    InkSample previous_{};
    bool hasPrevious_ = false;
};
