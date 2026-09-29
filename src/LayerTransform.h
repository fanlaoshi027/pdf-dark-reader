#pragma once
#include "LayerSystem.h"

class LayerTransform {
public:
    static POINT PdfToView(const PdfViewTransform& t, double x, double y);
    static POINT ViewToPdf(const PdfViewTransform& t, int x, int y);
    static double ClampScale(double scale);
};
