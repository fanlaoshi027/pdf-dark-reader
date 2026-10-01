#pragma once

#include "PdfViewController.h"

class PdfViewStateBridge {
public:
    explicit PdfViewStateBridge(PdfViewController& view) : view_(view) {}

    void PageCountChanged(int pageCount) { view_.Reset(pageCount); }
    void PageChanged(int page) { view_.SetPage(page); }
    void RenderSizeChanged(int width, int height) { view_.SetRenderSize(width, height); }
    void ZoomChanged(double zoom) { view_.SetZoom(zoom); }
    void ScrollChanged(int scrollY) { view_.SetScrollY(scrollY); }
    void FitWidthChanged(bool enabled) { view_.SetFitWidth(enabled); }
    void InvertChanged(bool enabled) { view_.SetInvert(enabled); }

private:
    PdfViewController& view_;
};
