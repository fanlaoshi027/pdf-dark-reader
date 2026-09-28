#include "LayerSystem.h"
#include <algorithm>
#include <cmath>
#include <windowsx.h>

namespace {
constexpr wchar_t kOverlayClass[] = L"PDFDarkReaderMosuanOverlay";
constexpr float kMinPressure = 0.08f;
constexpr float kMaxPressure = 1.00f;
constexpr float kMinWidthPdf = 0.75f;
constexpr float kMaxWidthPdf = 2.45f;
constexpr double kMinPointDistance = 0.35;

struct ViewPoint { double x; double y; float pressure; };

float ClampPressure(float p) {
    if (!std::isfinite(p) || p <= 0.0f) return 0.5f;
    return std::clamp(p, kMinPressure, kMaxPressure);
}

float PressureWidth(float p) {
    const float t = std::sqrt((ClampPressure(p) - kMinPressure) / (kMaxPressure - kMinPressure));
    return kMinWidthPdf + (kMaxWidthPdf - kMinWidthPdf) * t;
}

ViewPoint SmoothPoint(const std::vector<InkPoint>& pts, size_t i) {
    if (pts.size() < 3 || i == 0 || i + 1 >= pts.size()) {
        const auto& p = pts[i];
        return {p.pdfX, p.pdfY, p.pressure};
    }
    const auto& a = pts[i - 1]; const auto& b = pts[i]; const auto& c = pts[i + 1];
    return {
        (a.pdfX + 2.0 * b.pdfX + c.pdfX) * 0.25,
        (a.pdfY + 2.0 * b.pdfY + c.pdfY) * 0.25,
        (a.pressure + 2.0f * b.pressure + c.pressure) * 0.25f
    };
}
}

bool LayerSystem::Create(HWND parent) {
    parent_ = parent;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpfnWndProc = &LayerSystem::OverlayProc;
    wc.lpszClassName = kOverlayClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_CROSS);
    wc.hbrBackground = nullptr;
    RegisterClassExW(&wc);
    overlay_ = CreateWindowExW(WS_EX_NOACTIVATE, kOverlayClass, L"", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0, parent_, nullptr, GetModuleHandleW(nullptr), this);
    if (!overlay_) return false;
    UpdateHitTest();
    return true;
}

void LayerSystem::Resize(const RECT& viewport) {
    if (!overlay_) return;
    MoveWindow(overlay_, viewport.left, viewport.top,
        (std::max)(0L, viewport.right - viewport.left),
        (std::max)(0L, viewport.bottom - viewport.top), TRUE);
    ShowWindow(overlay_, mosuanVisible_ ? SW_SHOWNOACTIVATE : SW_HIDE);
    InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::SetTransform(const PdfViewTransform& transform) {
    transform_ = transform;
    if (overlay_) InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::SetMosuanVisible(bool visible) {
    mosuanVisible_ = visible;
    if (overlay_) { ShowWindow(overlay_, visible ? SW_SHOWNOACTIVATE : SW_HIDE); UpdateHitTest(); }
}

POINT LayerSystem::PdfToView(double pdfX, double pdfY) const {
    return POINT{static_cast<LONG>(std::lround(transform_.originX + pdfX * transform_.scale)),
                 static_cast<LONG>(std::lround(transform_.originY + pdfY * transform_.scale))};
}

POINT LayerSystem::ViewToPdf(int viewX, int viewY) const {
    const double scale = transform_.scale > 0.0 ? transform_.scale : 1.0;
    return POINT{static_cast<LONG>(std::lround((viewX - transform_.originX) / scale)),
                 static_cast<LONG>(std::lround((viewY - transform_.originY) / scale))};
}

void LayerSystem::SetPenEnabled(bool enabled) { penEnabled_ = enabled; UpdateHitTest(); }
void LayerSystem::ClearInk() { strokes_.clear(); penDown_ = false; InvalidateRect(overlay_, nullptr, FALSE); }
float LayerSystem::ClampPressure(float pressure) { return ::ClampPressure(pressure); }
float LayerSystem::PenWidthPdf(float pressure) { return ::PressureWidth(pressure); }

void LayerSystem::BeginPen(UINT32 pointerId, POINT screenPoint, float pressure) {
    if (!overlay_ || !mosuanVisible_ || !mosuanActive_ || !penEnabled_) return;
    POINT p = screenPoint; ScreenToClient(overlay_, &p);
    const POINT pdf = ViewToPdf(p.x, p.y);
    InkStroke stroke; stroke.points.push_back({static_cast<double>(pdf.x), static_cast<double>(pdf.y), ClampPressure(pressure)});
    strokes_.push_back(std::move(stroke)); activePointerId_ = pointerId; penDown_ = true; SetCapture(overlay_);
    InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::UpdatePen(UINT32 pointerId, POINT screenPoint, float pressure) {
    if (!penDown_ || pointerId != activePointerId_ || strokes_.empty()) return;
    POINT p = screenPoint; ScreenToClient(overlay_, &p); const POINT pdf = ViewToPdf(p.x, p.y);
    auto& stroke = strokes_.back();
    if (!stroke.points.empty()) {
        const auto& last = stroke.points.back(); const double dx = pdf.x - last.pdfX; const double dy = pdf.y - last.pdfY;
        if (dx * dx + dy * dy < kMinPointDistance * kMinPointDistance) return;
    }
    stroke.points.push_back({static_cast<double>(pdf.x), static_cast<double>(pdf.y), ClampPressure(pressure)});
    InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::EndPen(UINT32 pointerId) {
    if (!penDown_ || pointerId != activePointerId_) return;
    penDown_ = false; activePointerId_ = 0; ReleaseCapture(); InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::PaintOverlay(HDC hdc) {
    if (!mosuanVisible_) return;
    SetBkMode(hdc, TRANSPARENT);
    for (const auto& stroke : strokes_) {
        if (stroke.points.empty()) continue;
        std::vector<ViewPoint> pts; pts.reserve(stroke.points.size());
        for (size_t i = 0; i < stroke.points.size(); ++i) pts.push_back(SmoothPoint(stroke.points, i));
        HPEN pen = CreatePen(PS_SOLID, 1, penColor_);
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        for (size_t i = 1; i < pts.size(); ++i) {
            const POINT a = PdfToView(pts[i - 1].x, pts[i - 1].y);
            const POINT b = PdfToView(pts[i].x, pts[i].y);
            const int width = (std::max)(1, static_cast<int>(std::lround(PressureWidth(pts[i].pressure) * transform_.scale)));
            // GDI cannot vary a line's width point-by-point. Draw short pressure
            // segments with a round cap; this keeps the visual pressure change
            // smooth without changing the PDF coordinate data.
            HPEN segment = CreatePen(PS_SOLID, width, penColor_);
            HGDIOBJ old = SelectObject(hdc, segment);
            MoveToEx(hdc, a.x, a.y, nullptr); LineTo(hdc, b.x, b.y);
            SelectObject(hdc, old); DeleteObject(segment);
        }
        SelectObject(hdc, oldPen); DeleteObject(pen);
    }
}

void LayerSystem::UpdateHitTest() { if (overlay_) InvalidateRect(overlay_, nullptr, FALSE); }

LRESULT CALLBACK LayerSystem::OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<LayerSystem*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam); self = static_cast<LayerSystem*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);
    switch (message) {
    case WM_NCHITTEST: return (self->mosuanActive_ && self->penEnabled_) ? HTCLIENT : HTTRANSPARENT;
    case WM_POINTERDOWN: {
        const UINT32 id = GET_POINTERID_WPARAM(wParam); POINTER_INFO info{}; POINTER_PEN_INFO pen{};
        if (!GetPointerInfo(id, &info) || info.pointerType != PT_PEN || !GetPointerPenInfo(id, &pen)) return 0;
        self->BeginPen(id, info.ptPixelLocation, pen.pressure / 1024.0f); return 0;
    }
    case WM_POINTERUPDATE: {
        const UINT32 id = GET_POINTERID_WPARAM(wParam); if (!self->penDown_) return 0;
        POINTER_INFO info{}; POINTER_PEN_INFO pen{};
        if (!GetPointerInfo(id, &info) || !GetPointerPenInfo(id, &pen)) return 0;
        self->UpdatePen(id, info.ptPixelLocation, pen.pressure / 1024.0f); return 0;
    }
    case WM_POINTERUP: self->EndPen(GET_POINTERID_WPARAM(wParam)); return 0;
    case WM_CAPTURECHANGED: self->penDown_ = false; self->activePointerId_ = 0; return 0;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: { PAINTSTRUCT ps{}; HDC hdc = BeginPaint(hwnd, &ps); self->PaintOverlay(hdc); EndPaint(hwnd, &ps); return 0; }
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
