#pragma once

#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <vector>

struct PdfViewTransform {
    double scale = 1.0;
    int originX = 0;
    int originY = 0;
    int pageWidth = 0;
    int pageHeight = 0;
};

struct InkPoint { double pdfX = 0.0; double pdfY = 0.0; float pressure = 0.5f; };
struct InkStroke { std::vector<InkPoint> points; };

enum class MosuanTool { Pen, Line, Eraser, Lasso };
enum class LayerKind { Background, Pdf, Ink };

struct LayerItem {
    int id = 0;
    LayerKind kind = LayerKind::Ink;
    wchar_t name[64] = L"笔记";
    bool visible = true;
    bool locked = false;
};

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

    void SetTool(MosuanTool tool);
    MosuanTool Tool() const { return tool_; }
    void SetPenEnabled(bool enabled);
    bool PenEnabled() const { return penEnabled_; }
    void SetPenColor(COLORREF color) { penColor_ = color; Invalidate(); }
    COLORREF PenColor() const { return penColor_; }
    void SetPenWidth(float width) { penWidth_ = (std::max)(0.5f, width); Invalidate(); }
    float PenWidth() const { return penWidth_; }
    void SetDashMode(bool dashed) { dashMode_ = dashed; Invalidate(); }
    bool DashMode() const { return dashMode_; }
    void SetOneStrokeMode(bool enabled) { oneStrokeMode_ = enabled; }
    bool OneStrokeMode() const { return oneStrokeMode_; }

    POINT PdfToView(double pdfX, double pdfY) const;
    POINT ViewToPdf(int viewX, int viewY) const;

    void ResetDocumentLayers();
    int AddInkLayer(const wchar_t* name);
    int CreateNoteLayer(const wchar_t* name) { return AddInkLayer(name); }
    bool RemoveLayer(int id);
    bool SetActiveLayer(int id);
    int ActiveLayerId() const { return activeLayerId_; }
    int MosuanLayerId() const { return activeLayerId_; }
    const std::vector<LayerItem>& Layers() const { return layers_; }
    LayerItem* FindLayer(int id);
    void SetLayerVisible(int id, bool visible);
    void SetLayerLocked(int id, bool locked);

    void SetBackgroundVisible(bool visible) { backgroundVisible_ = visible; Invalidate(); }
    bool BackgroundVisible() const { return backgroundVisible_; }
    void SetPdfVisible(bool visible) { pdfVisible_ = visible; Invalidate(); }
    bool PdfVisible() const { return pdfVisible_; }
    void SetActiveLayerId(int id) { SetActiveLayer(id); }

    void ClearInk();
    void PaintOverlay(HDC hdc);

private:
    static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void UpdateHitTest();
    void Invalidate() { if (overlay_) InvalidateRect(overlay_, nullptr, FALSE); }
    void BeginPen(UINT32 pointerId, POINT screenPoint, float pressure);
    void UpdatePen(UINT32 pointerId, POINT screenPoint, float pressure);
    void EndPen(UINT32 pointerId);
    void EraseAt(POINT viewPoint);
    void FinishLasso();
    bool PointInLasso(const POINT& p) const;
    bool StrokeSelected(const InkStroke& stroke) const;
    static float PenWidthPdf(float pressure);
    static float ClampPressure(float pressure);

    HWND parent_ = nullptr;
    HWND overlay_ = nullptr;
    PdfViewTransform transform_;
    bool mosuanVisible_ = true;
    bool mosuanActive_ = true;
    bool penEnabled_ = true;
    bool backgroundVisible_ = true;
    bool pdfVisible_ = true;
    UINT32 activePointerId_ = 0;
    bool penDown_ = false;
    std::vector<LayerItem> layers_;
    int activeLayerId_ = 3;
    int nextLayerId_ = 4;
    std::vector<InkStroke> strokes_;
    std::vector<POINT> lassoPoints_;
    std::vector<size_t> selectedStrokes_;
    MosuanTool tool_ = MosuanTool::Pen;
    COLORREF penColor_ = RGB(35, 75, 150);
    float penWidth_ = 2.0f;
    bool dashMode_ = false;
    bool oneStrokeMode_ = true;
};
