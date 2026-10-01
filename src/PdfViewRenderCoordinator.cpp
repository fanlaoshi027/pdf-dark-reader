#include "PdfViewRenderCoordinator.h"

PdfRenderRequest PdfViewRenderCoordinator::MakeRequest(int pageIndex) const {
    const auto& s = state_.State();
    return {pageIndex, s.renderWidth, s.renderHeight};
}

bool PdfViewRenderCoordinator::RenderCurrent(std::vector<std::uint8_t>& pixels) {
    return RenderPage(state_.State().pageIndex, pixels);
}

bool PdfViewRenderCoordinator::RenderPage(int pageIndex,
                                          std::vector<std::uint8_t>& pixels) {
    const auto request = MakeRequest(pageIndex);
    if (request.pixelWidth <= 0 || request.pixelHeight <= 0) {
        pixels.clear();
        return false;
    }
    return renderer_.RenderPage(request.pageIndex,
                                request.pixelWidth,
                                request.pixelHeight,
                                pixels);
}
