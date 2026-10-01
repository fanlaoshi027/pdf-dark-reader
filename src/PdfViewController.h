#pragma once

#include "PdfViewStateController.h"
#include "PdfViewRenderCoordinator.h"
#include <cstdint>
#include <vector>

class PdfViewController {
public:
    PdfViewController(PdfViewStateController& state,
                      PdfViewRenderCoordinator& renderer)
        : state_(state), renderer_(renderer) {}

    void Reset(int pageCount);
    void SetPage(int pageIndex);
    void SetRenderSize(int width, int height);
    void SetZoom(double zoom);
    void SetScrollY(int scrollY);
    void SetFitWidth(bool enabled);
    void SetInvert(bool enabled);
    bool RenderCurrent(std::vector<std::uint8_t>& pixels);

    const PdfViewState& State() const { return state_.State(); }

private:
    PdfViewStateController& state_;
    PdfViewRenderCoordinator& renderer_;
};
