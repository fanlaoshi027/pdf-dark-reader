#pragma once

#include "InkRealtimeStroke.h"
#include "InkVectorGeometry.h"

class InkRealtimeGeometry {
public:
    InkStrokeGeometry BuildPreview(const InkRealtimeStroke& stroke) const;
};
