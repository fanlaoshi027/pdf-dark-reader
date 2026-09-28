#include "LayerSystem.h"
#include <algorithm>
#include <cmath>
#include <windowsx.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

namespace {
using namespace Gdiplus;
constexpr wchar_t kOverlayClass[] = L"PDFDarkReaderMosuanOverlay";
constexpr float kMinPressure = 0.08f;
constexpr float kMaxPressure = 1.00f;
constexpr float kMinWidthPdf = 0.70f;
constexpr float kMaxWidthPdf = 2.45f;
constexpr double kMinPointDistance = 0.22;

struct ViewPoint { double x; double y; float pressure; };

float ClampPressure(float p) {
    if (!std::isfinite(p) || p <= 0.0f) return 0.5f;
    return std::clamp(p, kMinPressure, kMaxPressure);
}

float PressureWidth(float p) {
    const float t = std::sqrt((ClampPressure(p) - kMinPressure) / (kMaxPressure - kMinPressure));
    return kMinWidthPdf + (kMaxWidthPdf - kMinWidthPdf) * t;
}

ViewPoint CatmullRom(const InkPoint& p0, const InkPoint& p1, const InkPoint& p2, const InkPoint& p3, double t) {
    const double t2 = t * t;
    const double t3 = t2 * t;
    const double x = 0.5 * ((2.0 * p1.pdfX) + (-p0.pdfX + p2.pdfX) * t +
        (2.0 * p0.pdfX - 5.0 * p1.pdfX + 4.0 * p2.pdfX - p3.pdfX) * t2 +
        (-p0.pdfX + 3.0 * p1.pdfX - 3.0 * p2.pdfX + p3.pdfX) * t3);
    const double y = 0.5 * ((2.0 * p1.pdfY) + (-p0.pdfY + p2.pdfY) * t +
        (2.0 * p0.pdfY - 5.0 * p1.pdfY + 4.0 * p2.pdfY - p3.pdfY) * t2 +
        (-p0.pdfY + 3.0 * p1.pdfY - 3.0 * p2.pdfY + p3.pdfY) * t3);
    const float pressure = static_cast<float>(
        0.5 * ((2.0 * p1.pressure) + (-p0.pressure + p2.pressure) * t +
        (2.0 * p0.pressure - 5.0 * p1.pressure + 4.0 * p2.pressure - p3.pressure) * t2 +
        (-p0.pressure + 3.0 * p1.pressure - 3.0 * p2.pressure + p3.pressure) * t3));
    return {x, y, ClampPressure(pressure)};
}

std::vector<ViewPoint> BuildSmoothStroke(const std::vector<InkPoint>& input) {
    std::vector<ViewPoint> out;
    if (input.empty()) return out;
    if (input.size() == 1) {
        out.push_back({input[0].pdfX, input[0].pdfY, ClampPressure(input[0].pressure)});
        return out;
    }

    out.reserve(input.size() * 4);
    for (size_t i = 0; i + 1 < input.size(); ++i) {
        const InkPoint& p0 = input[i == 0 ? i : i - 1];
        const InkPoint& p1 = input[i];
        const InkPoint& p2 = input[i + 1];
        const InkPoint& p3 = input[(i + 2 < input.size()) ? i + 2 : i + 1];

        const double dx = p2.pdfX - p1.pdfX;
        const double dy = p2.pdfY - p1.pdfY;
        const double distance = std::sqrt(dx * dx + dy * dy);
        const int steps = std::clamp(static_cast<int>(std::ceil(distance * 1.6)), 3, 32);
        for (int s = 0; s < steps; ++s) {
            const double t = static_cast<double>(s) / static_cast<double>(steps);
            out.push_back(CatmullRom(p0, p1, p2, p3, t));
        }
    }
    const auto& last = input.back();
    out.push_back({last.pdfX, last.pdfY, ClampPressure(last.pressure)});
    return out;
}
}

bool LayerSystem::Create(HWND parent) {
    parent_ = parent;

    // GDI+ provides real anti-aliased paths/round caps for the ink layer.
    // The PDF layer remains completely independent.
    static bool gdiplusStarted = false;
    if (!gdiplusStarted) {
        GdiplusStartupInput input;
        ULONG_PTR token = 0;
        if (GdiplusStartup(&token, &input, nullptr) == Ok) gdiplusStarted = true;
    }

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
    POINT p = screenPoint; ScreenToClient(overlay_, &p);
    const POINT pdf = ViewToPdf(p.x, p.y);
    auto& stroke = strokes_.back();
    if (!stroke.points.empty()) {
        const auto& last = stroke.points.back();
        const double dx = pdf.x - last.pdfX;
        const double dy = pdf.y - last.pdfY;
        if (dx * dx + dy * dy < kMinPointDistance * kMinPointDistance) return;
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

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    graphics.SetCompositingQuality(CompositingQualityHighQuality);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    const Color color(255, GetRValue(penColor_), GetGValue(penColor_), GetBValue(penColor_));

    for (const auto& stroke : strokes_) {
        if (stroke.points.empty()) continue;
        const auto points = BuildSmoothStroke(stroke.points);
        if (points.empty()) continue;

        if (points.size() == 1) {
            const auto& p = points.front();
            const PointF center(static_cast<REAL>(PdfToView(p.x, p.y).x),
                                static_cast<REAL>(PdfToView(p.x, p.y).y));
            const REAL width = static_cast<REAL>((std::max)(1.0, PressureWidth(p.pressure) * transform_.scale));
            SolidBrush brush(color);
            graphics.FillEllipse(&brush, center.X - width * 0.5f, center.Y - width * 0.5f, width, width);
            continue;
        }

        for (size_t i = 1; i < points.size(); ++i) {
            const auto& a = points[i - 1];
            const auto& b = points[i];
            const POINT va = PdfToView(a.x, a.y);
            const POINT vb = PdfToView(b.x, b.y);
            const REAL width = static_cast<REAL>((std::max)(1.0, PressureWidth((a.pressure + b.pressure) * 0.5f) * transform_.scale));

            Pen pen(color, width);
            pen.SetStartCap(LineCapRound);
            pen.SetEndCap(LineCapRound);
            pen.SetLineJoin(LineJoinRound);
            graphics.DrawLine(&pen, static_cast<REAL>(va.x), static_cast<REAL>(va.y),
                              static_cast<REAL>(vb.x), static_cast<REAL>(vb.y));
        }
    }
}

void LayerSystem::UpdateHitTest() { if (overlay_) InvalidateRect(overlay_, nullptr, FALSE); }

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
        return (self->mosuanActive_ && self->penEnabled_) ? HTCLIENT : HTTRANSPARENT;
    case WM_POINTERDOWN: {
        const UINT32 id = GET_POINTERID_WPARAM(wParam);
        POINTER_INFO info{};
        POINTER_PEN_INFO pen{};
        if (!GetPointerInfo(id, &info) || info.pointerType != PT_PEN || !GetPointerPenInfo(id, &pen)) return 0;
        self->BeginPen(id, info.ptPixelLocation, pen.pressure / 1024.0f);
        return 0;
    }
    case WM_POINTERUPDATE: {
        const UINT32 id = GET_POINTERID_WPARAM(wParam);
        if (!self->penDown_) return 0;
        POINTER_INFO info{};
        POINTER_PEN_INFO pen{};
        if (!GetPointerInfo(id, &info) || !GetPointerPenInfo(id, &pen)) return 0;
        self->UpdatePen(id, info.ptPixelLocation, pen.pressure / 1024.0f);
        return 0;
    }
    case WM_POINTERUP:
        self->EndPen(GET_POINTERID_WPARAM(wParam));
        return 0;
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
