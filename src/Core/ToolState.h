#pragma once
#include "InkTypes.h"
#include "BrushState.h"

struct ToolState {
    MosuanTool activeTool = MosuanTool::Pen;
    BrushState brush{};
    bool rulerVisible = false;
    bool lassoActive = false;
    bool eraserActive = false;
};
