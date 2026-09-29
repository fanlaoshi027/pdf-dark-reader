#pragma once
#include "BrushState.h"
#include "CoordinateTransform.h"
#include "ToolState.h"

// Platform-neutral ink engine facade. Platform adapters feed normalized samples here.
class InkEngine {
public:
    void SetBrush(const BrushState& brush) { toolState_.brush = brush; toolState_.activeTool = brush.tool; }
    const BrushState& Brush() const { return toolState_.brush; }

    void SetTool(MosuanTool tool) { toolState_.activeTool = tool; toolState_.brush.tool = tool; }
    MosuanTool Tool() const { return toolState_.activeTool; }

    void SetTransform(const PdfViewTransform& transform) { transform_.Set(transform); }
    const CoordinateTransform& Transform() const { return transform_; }

    // Normalize platform samples into PDF page-space coordinates.
    InkPoint ToPagePoint(double viewX, double viewY, float pressure = 1.0f) const {
        InkPoint point{};
        point.pdfX = transform_.ViewToPdfX(viewX);
        point.pdfY = transform_.ViewToPdfY(viewY);
        point.pressure = pressure;
        return point;
    }

private:
    ToolState toolState_{};
    CoordinateTransform transform_{};
};
