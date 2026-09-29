#pragma once
#include "InkTypes.h"

// Platform-neutral conversion between PDF page coordinates and view coordinates.
class CoordinateTransform {
public:
    void Set(const PdfViewTransform& transform) { transform_ = transform; }
    const PdfViewTransform& Get() const { return transform_; }

    double PdfToViewX(double x) const { return x * transform_.scale + transform_.offsetX; }
    double PdfToViewY(double y) const { return y * transform_.scale + transform_.offsetY; }
    double ViewToPdfX(double x) const { return (x - transform_.offsetX) / transform_.scale; }
    double ViewToPdfY(double y) const { return (y - transform_.offsetY) / transform_.scale; }

private:
    PdfViewTransform transform_{};
};
