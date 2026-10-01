#include "DocumentInkViewportSync.h"

void DocumentInkViewportSync::SetZoom(double zoom) {
    auto mapper = binding_.Mapper();
    mapper.zoom = zoom > 0.001 ? zoom : 0.001;
    binding_.SetMapper(mapper);
}

void DocumentInkViewportSync::SetPan(double x, double y) {
    auto mapper = binding_.Mapper();
    mapper.panX = x;
    mapper.panY = y;
    binding_.SetMapper(mapper);
}

void DocumentInkViewportSync::SetPage(int page) {
    binding_.SetPage(page);
}
