#pragma once

#include "InkRenderPoint.h"
#include <vector>

class InkStrokeCap {
public:
    static void AddRoundStart(std::vector<InkRenderPoint>& points);
    static void AddRoundEnd(std::vector<InkRenderPoint>& points);
};
