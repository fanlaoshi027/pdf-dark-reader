#include "PdfViewRenderCoordinator.h"

PdfRenderRequest PdfViewRenderCoordinator::MakeRequest(int pageIndex) const {
    const auto& s = state_.State();
    return {pageIndex, s.renderWidth, s.renderHeight};
}

HBITMAP PdfViewRenderCoordinator::RenderCurrent() {
    return RenderPage(state_.State().pageIndex);
}

HBITMAP PdfViewRenderCoordinator::RenderPage(int pageIndex) {
    const auto request = MakeRequest(pageIndex);
    if (request.pixelWidth <= 0 || request.pixelHeight <= 0) return nullptr;
    return renderer_.RenderPage(request.pageIndex,
                                request.pixelWidth,
                                request.pixelHeight);
}
