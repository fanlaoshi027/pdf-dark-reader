#include "PdfViewport.h"

void PdfViewport::SetPage(int pageIndex, double width, double height) {
    transform_.State().pageIndex = pageIndex < 0 ? 0 : pageIndex;
    transform_.State().pageWidth = width > 0.0 ? width : 0.0;
    transform_.State().pageHeight = height > 0.0 ? height : 0.0;
}

void PdfViewport::Pan(double dx, double dy) {
    transform_.Pan(dx, dy);
}

void PdfViewport::Zoom(double scale, double centerX, double centerY) {
    transform_.ZoomAround(scale, centerX, centerY);
}

void PdfViewport::ResetPosition() {
    transform_.ResetOrigin();
}
