#include "PdfViewController.h"

void PdfViewController::Reset(int pageCount) {
    state_.Reset(pageCount);
}

void PdfViewController::SetPage(int pageIndex) {
    state_.SetPage(pageIndex);
}

void PdfViewController::SetRenderSize(int width, int height) {
    state_.SetRenderSize(width, height);
}

void PdfViewController::SetZoom(double zoom) {
    state_.SetZoom(zoom);
}

void PdfViewController::SetScrollY(int scrollY) {
    state_.SetScrollY(scrollY);
}

void PdfViewController::SetFitWidth(bool enabled) {
    state_.SetFitWidth(enabled);
}

void PdfViewController::SetInvert(bool enabled) {
    state_.SetInvert(enabled);
}

bool PdfViewController::RenderCurrent(std::vector<std::uint8_t>& pixels) {
    return renderer_.RenderCurrent(pixels);
}
