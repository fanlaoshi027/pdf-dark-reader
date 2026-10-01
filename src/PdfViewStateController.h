#pragma once

#include "PdfViewState.h"

class PdfViewStateController {
public:
    PdfViewState& State() { return state_; }
    const PdfViewState& State() const { return state_; }

    void Reset(int pageCount);
    void SetPage(int pageIndex);
    void SetRenderSize(int width, int height);
    void SetZoom(double zoom);
    void SetScrollY(int scrollY);
    void SetFitWidth(bool enabled);
    void SetInvert(bool enabled);

private:
    PdfViewState state_{};
};
