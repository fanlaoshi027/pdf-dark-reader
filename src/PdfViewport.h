#pragma once

#include "ViewTransformController.h"

class PdfViewport {
public:
    explicit PdfViewport(ViewTransformController& transform) : transform_(transform) {}

    void SetPage(int pageIndex, double width, double height);
    void Pan(double dx, double dy);
    void Zoom(double scale, double centerX, double centerY);
    void ResetPosition();

    const ViewTransformState& Transform() const { return transform_.State(); }

private:
    ViewTransformController& transform_;
};
