#pragma once
#include "InkSample.h"
#include "InkStrokeSession.h"

class InkInputAdapter {
public:
    void BeginStroke(std::uint32_t color, float width, bool dashed);
    void PushSample(double x, double y, float pressure, std::uint64_t timestamp);
    VectorStroke EndStroke();
    void CancelStroke();
    bool Active() const { return session_.Active(); }
private:
    InkStrokeSession session_;
};
