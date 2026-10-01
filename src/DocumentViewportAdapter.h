#pragma once

#include "DocumentViewport.h"
#include "ViewTransformController.h"

class DocumentViewportAdapter {
public:
    explicit DocumentViewportAdapter(ViewTransformController& transform)
        : transform_(transform) {}

    void SyncFromTransform();
    void SyncToTransform();

    DocumentViewport& Viewport() { return viewport_; }
    const DocumentViewport& Viewport() const { return viewport_; }

private:
    ViewTransformController& transform_;
    DocumentViewport viewport_;
};
