#include "LayerSystem.h"
#include <algorithm>
#include <cmath>
#include <windowsx.h>

namespace {
constexpr wchar_t kOverlayClass[] = L"PDFDarkReaderMosuanOverlay";
constexpr float kMinPressure = 0.10f;
constexpr float kMaxPressure = 1.00f;
constexpr float kMinWidthPdf = 0.90f;
constexpr float kMaxWidthPdf = 2.70f;

COLORREF WithAlphaIgnored(COLORREF color) { return color; }
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

    // The overlay occupies only the document viewport. When pen input is
    // active it receives pointer messages; when disabled it becomes hit-test
    // transparent and PDF scrolling/navigation remains untouched.
    overlay_ = CreateWindowExW(
        WS_EX_NOACTIVATE,
        kOverlayClass, L"", WS_CHILD | WS_VISIBLE,
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
    if (overlay_) {
        ShowWindow(overlay_, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
        UpdateHitTest();
    }
}

POINT LayerSystem::PdfToView(double pdfX, double pdfY) const {
    return POINT{
        static_cast<LONG>(std::lround(transform_.originX + pdfX * transform_.scale)),
        static_cast<LONG>(std::lround(transform_.originY + pdfY * transform_.scale))
    };
}

POINT LayerSystem::ViewToPdf(int viewX, int viewY) const {
    const double scale = transform_.scale > 0.0 ? transform_.scale : 1.0;
    return POINT{
        static_cast<LONG>(std::lround((viewX - transform_.originX) / scale)),
        static_cast<LONG>(std::lround((viewY - transform_.originY) / scale))
    };
}

void LayerSystem::SetPenEnabled(bool enabled) {
    penEnabled_ = enabled;
    UpdateHitTest();
}

void LayerSystem::ClearInk() {
    strokes_.clear();
    penDown_ = false;
    InvalidateRect(overlay_, nullptr, FALSE);
}

float LayerSystem::ClampPressure(float pressure) {
    if (!std::isfinite(pressure) || pressure <= 0.0f) return 0.5f;
    return std::clamp(pressure, kMinPressure, kMaxPressure);
}

float LayerSystem::PenWidthPdf(float pressure) {
    const float p = ClampPressure(pressure);
    // Mild pressure curve: light touch stays thin while heavy pressure grows
    // smoothly without becoming a marker.
    const float t = std::sqrt((p - kMinPressure) / (kMaxPressure - kMinPressure));
    return kMinWidthPdf + (kMaxWidthPdf - kMinWidthPdf) * t;
}

void LayerSystem::BeginPen(UINT32 pointerId, POINT screenPoint, float pressure) {
    if (!overlay_ || !mosuanVisible_ || !mosuanActive_ || !penEnabled_) return;
    POINT p = screenPoint;
    ScreenToClient(overlay_, &p);
    const POINT pdf = ViewToPdf(p.x, p.y);
    InkStroke stroke;
    stroke.points.push_back({static_cast<double>(pdf.x), static_cast<double>(pdf.y), ClampPressure(pressure)});
    strokes_.push_back(std::move(stroke));
    activePointerId_ = pointerId;
    penDown_ = true;
    SetCapture(overlay_);
    InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::UpdatePen(UINT32 pointerId, POINT screenPoint, float pressure) {
    if (!penDown_ || pointerId != activePointerId_ || strokes_.empty()) return;
    POINT p = screenPoint;
    ScreenToClient(overlay_, &p);
    const POINT pdf = ViewToPdf(p.x, p.y);
    auto& stroke = strokes_.back();
    if (!stroke.points.empty()) {
        const auto& last = stroke.points.back();
        const double dx = pdf.x - last.pdfX;
        const double dy = pdf.y - last.pdfY;
        // Ignore ultra-small pointer jitter, but keep the first movement so
        // slow handwriting remains continuous.
        if ((dx * dx + dy * dy) < 0.20) return;
    }
    stroke.points.push_back({static_cast<double>(pdf.x), static_cast<double>(pdf.y), ClampPressure(pressure)});
    InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::EndPen(UINT32 pointerId) {
    if (!penDown_ || pointerId != activePointerId_) return;
    penDown_ = false;
    activePointerId_ = 0;
    ReleaseCapture();
    InvalidateRect(overlay_, nullptr, FALSE);
}

void LayerSystem::PaintOverlay(HDC hdc) {
    if (!mosuanVisible_) return;
    SetBkMode(hdc, TRANSPARENT);
    SetROP2(hdc, R2_COPYPEN);

    for (const auto& stroke : strokes_) {
        if (stroke.points.empty()) continue;
        for (size_t i = 0; i < stroke.points.size(); ++i) {
            const auto& point = stroke.points[i];
            const POINT view = PdfToView(point.pdfX, point.pdfY);
            const int radius = (std::max)(1, static_cast<int>(std::lround(PenWidthPdf(point.pressure) * transform_.scale * 0.5)));
            const int diameter = radius * 2;
            HBRUSH brush = CreateSolidBrush(WithAlphaIgnored(penColor_));
            HPEN pen = CreatePen(PS_SOLID, (std::max)(1, radius * 2), penColor_);
            HGDIOBJ oldBrush = SelectObject(hdc, brush);
            HGDIOBJ oldPen = SelectObject(hdc, pen);
            if (i > 0) {
                const auto& previous = stroke.points[i - 1];
                const POINT prev = PdfToView(previous.pdfX, previous.pdfY);
                MoveToEx(hdc, prev.x, prev.y, nullptr);
                LineTo(hdc, view.x, view.y);
            }
            Ellipse(hdc, view.x - radius, view.y - radius,
                    view.x + radius + 1, view.y + radius + 1);
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(pen);
            DeleteObject(brush);
            (void)diameter;
        }
    }
}

void LayerSystem::UpdateHitTest() {
    if (!overlay_) return;
    InvalidateRect(overlay_, nullptr, FALSE);
}

LRESULT CALLBACK LayerSystem::OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<LayerSystem*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<LayerSystem*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);

    switch (message) {
    case WM_NCHITTEST:
        // Pen input must reach the overlay. For inactive mode, let the parent
        // receive normal mouse interaction such as scrolling.
        return (self->mosuanActive_ && self->penEnabled_) ? HTCLIENT : HTTRANSPARENT;

    case WM_POINTERDOWN: {
        const UINT32 id = GET_POINTERID_WPARAM(wParam);
        POINTER_INFO info{};
        if (!GetPointerInfo(id, &info)) return 0;
        if (info.pointerType != PT_PEN) return 0;
        POINTER_PEN_INFO pen{};
        if (!GetPointerPenInfo(id, &pen)) return 0;
        POINT screen = info.ptPixelLocation;
        self->BeginPen(id, screen, pen.pressure / 1024.0f);
        return 0;
    }
    case WM_POINTERUPDATE: {
        const UINT32 id = GET_POINTERID_WPARAM(wParam);
        if (!self->penDown_) return 0;
        POINTER_INFO info{};
        if (!GetPointerInfo(id, &info)) return 0;
        POINTER_PEN_INFO pen{};
        if (!GetPointerPenInfo(id, &pen)) return 0;
        self->UpdatePen(id, info.ptPixelLocation, pen.pressure / 1024.0f);
        return 0;
    }
    case WM_POINTERUP: {
        self->EndPen(GET_POINTERID_WPARAM(wParam));
        return 0;
    }
    case WM_CAPTURECHANGED:
        self->penDown_ = false;
        self->activePointerId_ = 0;
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        self->PaintOverlay(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
