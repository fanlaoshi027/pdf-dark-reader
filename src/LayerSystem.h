#pragma once

#include <windows.h>
#include <cstdint>
#include <vector>
#include "WindowsInkVectorBridge.h"
#include "ViewTransform.h"

using PdfViewTransform = ViewTransform;

struct InkPoint { double pdfX = 0.0; double pdfY = 0.0; float pressure = 0.5f; };
struct InkStroke { std::vector<InkPoint> points; };
enum class MosuanTool { Pen, Line, Eraser, Lasso };

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
    POINT PdfToView(double pdfX,double pdfY) const;
    POINT ViewToPdf(int viewX,int viewY) const;
    void SetTool(MosuanTool tool);
    MosuanTool Tool() const { return tool_; }
    void SetPenEnabled(bool enabled);
    bool PenEnabled() const { return penEnabled_; }
    bool NativeInkActive() const { return vectorInk_.Active(); }
    void ClearInk();
    void PaintOverlay(HDC hdc);

private:
    static LRESULT CALLBACK OverlayProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
    void UpdateHitTest();
    void UpdateNativeInkMode();
    void BeginPen(UINT32 pointerId,POINT screenPoint,float pressure);
    void UpdatePen(UINT32 pointerId,POINT screenPoint,float pressure);
    void EndPen(UINT32 pointerId);
    void EraseAt(POINT viewPoint);
    void FinishLasso();
    bool PointInLasso(const POINT& p) const;
    bool StrokeSelected(const InkStroke& stroke) const;
    static float PenWidthPdf(float pressure);
    static float ClampPressure(float pressure);
    void SyncVectorInkStroke();

    HWND parent_ = nullptr;
    HWND overlay_ = nullptr;
    PdfViewTransform transform_;
    bool mosuanVisible_ = true;
    bool mosuanActive_ = true;
    bool penEnabled_ = true;
    UINT32 activePointerId_ = 0;
    bool penDown_ = false;
    std::vector<InkStroke> strokes_;
    std::vector<POINT> lassoPoints_;
    std::vector<size_t> selectedStrokes_;
    MosuanTool tool_ = MosuanTool::Pen;
    COLORREF penColor_ = RGB(35,75,150);
    WindowsInkVectorBridge vectorInk_;
};
