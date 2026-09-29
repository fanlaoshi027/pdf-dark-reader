#pragma once
#include <windows.h>
#include <vector>
#include "InkDocument.h"
#include "StrokeDynamics.h"
#include "StrokeRenderer.h"

class InkEngine {
public:
    void SetDocument(InkDocument* document) { document_ = document; }
    void SetTransform(const PdfViewTransform& transform) { transform_ = transform; }
    void SetLayer(int layerId) { activeLayerId_ = layerId; }
    void SetStyle(const StrokeStyle& style) { style_ = style; }
    void SetTool(MosuanTool tool) { tool_ = tool; }

    bool Begin(POINT viewPoint, float pressure = 0.5f);
    bool Move(POINT viewPoint, float pressure = 0.5f);
    bool End(POINT viewPoint, float pressure = 0.5f);
    bool EraseAt(POINT viewPoint, float radius = 16.0f);
    bool AddLassoPoint(POINT viewPoint);
    bool FinishLasso();
    void Cancel();
    void Draw(HDC hdc) const;

    bool IsDrawing() const { return drawing_; }
    bool IsSelected(size_t index) const;
    const std::vector<size_t>& Selection() const { return selectedStrokes_; }

private:
    InkPoint ToInkPoint(POINT viewPoint, float pressure) const;
    InkDocument* document_ = nullptr;
    PdfViewTransform transform_;
    StrokeStyle style_;
    MosuanTool tool_ = MosuanTool::Pen;
    int activeLayerId_ = 3;
    bool drawing_ = false;
    InkStroke currentStroke_;
    POINT lineStart_{};
    POINT lineEnd_{};
    std::vector<POINT> lasso_;
    std::vector<size_t> selectedStrokes_;
};
