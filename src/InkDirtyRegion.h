#pragma once
#include <windows.h>

class InkDirtyRegion {
public:
    void Reset();
    void Include(double x, double y, double radius);
    RECT Consume();
    bool Empty() const { return empty_; }
private:
    RECT rect_{0,0,0,0};
    bool empty_ = true;
};
