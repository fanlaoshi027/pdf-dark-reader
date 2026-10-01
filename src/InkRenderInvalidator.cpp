#include "InkRenderInvalidator.h"

void InkRenderInvalidator::Include(double x, double y, double radius) {
    region_.Include(x, y, radius);
}

void InkRenderInvalidator::Invalidate(HWND hwnd) {
    if (!hwnd || region_.Empty()) return;
    const RECT rect = region_.Consume();
    InvalidateRect(hwnd, &rect, FALSE);
}

void InkRenderInvalidator::Reset() {
    region_.Reset();
}
