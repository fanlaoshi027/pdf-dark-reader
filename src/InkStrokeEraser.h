#pragma once
#include "InkStrokeStore.h"
#include "InkStrokeHitTest.h"

class InkStrokeEraser {
public:
    bool EraseAt(InkStrokeStore& store, double x, double y, double radius);
private:
    InkStrokeHitTest hitTest_;
};
