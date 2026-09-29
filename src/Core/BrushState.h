#pragma once
#include <cstdint>
#include "InkTypes.h"

struct BrushState {
    uint32_t color = 0x00234B96;
    float width = 2.0f;
    bool dashed = false;
    bool oneStroke = true;
    MosuanTool tool = MosuanTool::Pen;
};
