#pragma once

#include <windows.h>

struct PdfViewTransform {
    double scale = 1.0;
    int originX = 0;
    int originY = 0;
    int pageWidth = 0;
    int pageHeight = 0;
};

// Two independent layers with one shared PDF/view coordinate system:
//   bottom: PDF document
//   top:    Mosuan (墨算) ink/annotation layer
// The top layer is intentionally independent of PDF rendering. Future
// Windows Ink/Pen input can write into it without changing the PDF.
class LayerSystem {
public:
    bool Create(HWND parent);
    void Resize(const RECT& viewport);
    void SetTransform(const PdfViewTransform& transform);
    const PdfViewTransform& Transform() const { return transform_; }

    void SetMosuanVisible(bool visible);
    bool MosuanVisible() const { return mosuanVisible_; }
    void SetMosuanActive(bool active) { mosuanActive_ = active; }
    bool MosuanActive() const { return mosuanActive_; }

    POINT PdfToView(double pdfX, double pdfY) const;
    POINT ViewToPdf(int viewX, int viewY) const;

    void PaintOverlay(HDC hdc);

private:
    static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND parent_ = nullptr;
    HWND overlay_ = nullptr;
    PdfViewTransform transform_;
    bool mosuanVisible_ = true;
    bool mosuanActive_ = true;
};
