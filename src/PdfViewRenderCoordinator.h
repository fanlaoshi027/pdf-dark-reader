#pragma once

#include "PdfRenderController.h"
#include "PdfRenderRequest.h"
#include "PdfViewStateController.h"

class PdfViewRenderCoordinator {
public:
    PdfViewRenderCoordinator(PdfViewStateController& state,
                             PdfRenderController& renderer)
        : state_(state), renderer_(renderer) {}

    HBITMAP RenderCurrent();
    HBITMAP RenderPage(int pageIndex);

private:
    PdfRenderRequest MakeRequest(int pageIndex) const;

    PdfViewStateController& state_;
    PdfRenderController& renderer_;
};
