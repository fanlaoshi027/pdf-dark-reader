#pragma once

#include "InkRenderSnapshot.h"
#include "InkRealtimeGeometry.h"

struct InkRenderFrame {
    InkRenderSnapshot committed;
    InkStrokeGeometry preview;
};
