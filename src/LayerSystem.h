#pragma once

#include <windows.h>
#include <cstdint>
#include <vector>

struct PdfViewTransform {
    double scale = 1.0;
    int originX = 0;
    int originY = 0;
    int pageWidth = 0;
    int pageHeight = 0;
};

struct InkPoint {
    double pdfX = 0.0;
    double pdfY = 0.0;
    float pressure = 0.5f;
};

struct InkStroke {
    std::vector<InkPoint> points;
};

// Two independent layers with one shared PDF/view coordinate system:
//   bottom: PDF document
//   top:    Mosuan (墨算) ink/annotation layer
// The ink is stored in PDF coordinates, so zooming/scrolling never changes
// where a stroke belongs on the page.
class LayerSystem {
public:
    bool Create(HWND parent);
    void Resize(const RECT& viewport);
    void SetTransform(const PdfViewTransform& transform);
    const PdfViewTransform& Transform() const { return transform_; }

    void SetMosuanVisible(bool visible);
    bool MosuanVisible() const { return mosuanVisible_; }
    void SetMosuanActive(bool active) { mosuanActive_ = active; UpdateHitTest(); }
    bool MosuanActive() const { return mosuanActive_; }

    POINT PdfToView(double pdfX, double pdfY) const;
    POINT ViewToPdf(int viewX, int viewY) const;

    void SetPenEnabled(bool enabled);
    bool PenEnabled() const { return penEnabled_; }
    void ClearInk();

    void PaintOverlay(HDC hdc);

private:
    static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void UpdateHitTest();
    void BeginPen(UINT32 pointerId, POINT screenPoint, float pressure);
    void UpdatePen(UINT32 pointerId, POINT screenPoint, float pressure);
    void EndPen(UINT32 pointerId);
    static float PenWidthPdf(float pressure);
    static float ClampPressure(float pressure);

    HWND parent_ = nullptr;
    HWND overlay_ = nullptr;
    PdfViewTransform transform_;
    bool mosuanVisible_ = true;
    bool mosuanActive_ = true;
    bool penEnabled_ = true;
    UINT32 activePointerId_ = 0;
    bool penDown_ = false;
    std::vector<InkStroke> strokes_;
    COLORREF penColor_ = RGB(35, 75, 150);
};
