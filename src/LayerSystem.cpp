#include "LayerSystem.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr wchar_t kOverlayClass[] = L"PDFDarkReaderMosuanOverlay";
}

bool LayerSystem::Create(HWND parent) {
    parent_ = parent;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpfnWndProc = &LayerSystem::OverlayProc;
    wc.lpszClassName = kOverlayClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    RegisterClassExW(&wc);

    overlay_ = CreateWindowExW(
        WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        kOverlayClass, L"", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0, parent_, nullptr, GetModuleHandleW(nullptr), this);
    return overlay_ != nullptr;
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
    if (overlay_) ShowWindow(overlay_, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
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

void LayerSystem::PaintOverlay(HDC hdc) {
    // Intentionally transparent for now. The future Mosuan renderer will draw
    // strokes here; no PDF pixels are modified by the annotation layer.
    (void)hdc;
}

LRESULT CALLBACK LayerSystem::OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<LayerSystem*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<LayerSystem*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        if (self && self->mosuanVisible_) self->PaintOverlay(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
