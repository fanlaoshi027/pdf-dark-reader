#pragma once
#include <vector>
#include "BrushState.h"
#include "CoordinateTransform.h"
#include "ToolState.h"

// Platform-neutral ink engine facade. Platform adapters feed normalized InkPoint samples here.
class InkEngine {
public:
    void SetBrush(const BrushState& brush) { toolState_.brush = brush; toolState_.activeTool = brush.tool; }
    const BrushState& Brush() const { return toolState_.brush; }

    void SetTool(MosuanTool tool) { toolState_.activeTool = tool; toolState_.brush.tool = tool; }
    MosuanTool Tool() const { return toolState_.activeTool; }

    void SetTransform(const PdfViewTransform& transform) { transform_.Set(transform); }
    const CoordinateTransform& Transform() const { return transform_; }

    // Normalize incoming platform samples into page-space points.
    InkPoint ToPagePoint(double viewX, double viewY, float pressure = 1.0f, uint64_t timestamp = 0) const {
        InkPoint point{};
        point.x = transform_.ViewToPdfX(viewX);
        point.y = transform_.ViewToPdfY(viewY);
        point.pressure = pressure;
        point.timestamp = timestamp;
        return point;
    }

private:
    ToolState toolState_{};
    CoordinateTransform transform_{};
};
