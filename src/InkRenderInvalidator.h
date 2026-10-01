#pragma once
#include "InkDirtyRegion.h"
#include <windows.h>

class InkRenderInvalidator {
public:
    void Include(double x, double y, double radius);
    void Invalidate(HWND hwnd);
    void Reset();
private:
    InkDirtyRegion region_;
};
