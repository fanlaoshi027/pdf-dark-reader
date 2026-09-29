#pragma once
#include "InkDocument.h"
#include "LayerSystem.h"
#include "StrokeDynamics.h"

class InkPageController {
public:
    void SetDocument(InkDocument* document) { document_ = document; }
    void SetLayers(LayerSystem* layers) { layers_ = layers; }
    void SetPage(int pageIndex);
    void SetStyle(const StrokeStyle& style) { style_ = style; }
    void Begin(POINT viewPoint, float pressure);
    void Move(POINT viewPoint, float pressure);
    void End(POINT viewPoint, float pressure);
    void BeginLine(POINT viewPoint);
    void EndLine(POINT viewPoint);
    void Cancel();
    bool Drawing() const { return drawing_; }
    bool LineMode() const { return lineMode_; }
    const InkStroke& CurrentStroke() const { return currentStroke_; }
    POINT LineStart() const { return lineStart_; }

private:
    InkDocument* document_ = nullptr;
    LayerSystem* layers_ = nullptr;
    StrokeStyle style_;
    InkStroke currentStroke_;
    POINT lineStart_{};
    bool drawing_ = false;
    bool lineMode_ = false;
};
