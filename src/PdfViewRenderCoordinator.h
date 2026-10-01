#pragma once

#include "PdfRenderController.h"
#include "PdfRenderRequest.h"
#include "PdfViewStateController.h"
#include <cstdint>
#include <vector>

class PdfViewRenderCoordinator {
public:
    PdfViewRenderCoordinator(PdfViewStateController& state,
                             PdfRenderController& renderer)
        : state_(state), renderer_(renderer) {}

    bool RenderCurrent(std::vector<std::uint8_t>& pixels);
    bool RenderPage(int pageIndex, std::vector<std::uint8_t>& pixels);

private:
    PdfRenderRequest MakeRequest(int pageIndex) const;

    PdfViewStateController& state_;
    PdfRenderController& renderer_;
};
