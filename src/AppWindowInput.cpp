#include "AppWindowInput.h"
#include "AppWindow.h"
#include "AppWindowPdf.h"
#include "MosuanFavoritesRail.h"
#include "PdfThumbnailRail.h"
#include <windowsx.h>

namespace {
constexpr int kToolbarHeight = 70;

bool InPage(AppWindow& app, int x, int y) {
    if (!app.pdf().IsOpen() || y < kToolbarHeight) return false;
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    return x >= PdfThumbnailRail::kWidth && x < rc.right - MosuanFavoritesRail::kWidth && y < rc.bottom;
}

float PointerPressure(WPARAM wParam) {
    const UINT32 id = GET_POINTERID_WPARAM(wParam);
    POINTER_INFO info{};
    POINTER_PEN_INFO pen{};
    if (GetPointerInfo(id, &info) && info.pointerType == PT_PEN && GetPointerPenInfo(id, &pen)) {
        return static_cast<float>(pen.pressure) / 1024.0f;
    }
    return 0.5f;
}
}

LRESULT AppWindowInput::HandleKey(AppWindow& app, WPARAM key) {
    if (key == 'O' && (GetKeyState(VK_CONTROL) & 0x8000)) { AppWindowPdf::Open(app); return 0; }
    if (key == 'I' && app.pdf().IsOpen()) { app.SetInvertState(!app.invertEnabled()); AppWindowPdf::Render(app); return 0; }
    if (key == VK_LEFT) { AppWindowPdf::GoPage(app, -1); return 0; }
    if (key == VK_RIGHT) { AppWindowPdf::GoPage(app, 1); return 0; }
    if (key == VK_UP) { AppWindowPdf::Scroll(app, -80); return 0; }
    if (key == VK_DOWN) { AppWindowPdf::Scroll(app, 80); return 0; }
    if (key == VK_PRIOR) { AppWindowPdf::Scroll(app, -400); return 0; }
    if (key == VK_NEXT) { AppWindowPdf::Scroll(app, 400); return 0; }
    return 1;
}

LRESULT AppWindowInput::HandleMouseWheel(AppWindow& app, WPARAM wParam) {
    const int steps = -(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
    AppWindowPdf::Scroll(app, steps * 90);
    return 0;
}

LRESULT AppWindowInput::HandleVScroll(AppWindow& app, WPARAM wParam) {
    switch (LOWORD(wParam)) {
    case SB_LINEUP: AppWindowPdf::Scroll(app, -60); break;
    case SB_LINEDOWN: AppWindowPdf::Scroll(app, 60); break;
    case SB_PAGEUP: AppWindowPdf::Scroll(app, -400); break;
    case SB_PAGEDOWN: AppWindowPdf::Scroll(app, 400); break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK: {
        SCROLLINFO si{}; si.cbSize = sizeof(si); si.fMask = SIF_TRACKPOS;
        GetScrollInfo(app.hwnd(), SB_VERT, &si);
        app.SetScrollY(si.nTrackPos);
        AppWindowPdf::Render(app);
        break;
    }
    default: break;
    }
    return 0;
}

LRESULT AppWindowInput::HandleLButtonDown(AppWindow& app, int x, int y, WPARAM) {
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    std::size_t slot = 0; bool save = false;
    if (MosuanFavoritesRail::HitTest(rc, x, y, slot, save)) {
        MosuanFavoritesRail::Activate(app, slot, save);
        return 0;
    }
    int page = -1;
    if (PdfThumbnailRail::HitTest(app, x, y, page)) {
        app.SetPageIndex(page);
        app.SetScrollY(0);
        AppWindowPdf::Render(app);
        app.Refresh();
        return 0;
    }
    if (!InPage(app, x, y)) return 1;
    SetCapture(app.hwnd());
    app.ApplyInkState();
    app.Ink().Begin(POINT{x, y}, 0.5f);
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandleMouseMove(AppWindow& app, int x, int y, WPARAM) {
    if (GetCapture() != app.hwnd()) return 1;
    app.Ink().Move(POINT{x, y}, 0.5f);
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandleLButtonUp(AppWindow& app, int x, int y, WPARAM) {
    if (GetCapture() != app.hwnd()) return 1;
    app.Ink().End(POINT{x, y}, 0.5f);
    ReleaseCapture();
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandlePointerDown(AppWindow& app, WPARAM wParam, LPARAM lParam) {
    const POINT p{static_cast<LONG>(GET_X_LPARAM(lParam)), static_cast<LONG>(GET_Y_LPARAM(lParam))};
    if (!InPage(app, p.x, p.y)) return 1;
    SetCapture(app.hwnd());
    app.ApplyInkState();
    app.Ink().Begin(p, PointerPressure(wParam));
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandlePointerUpdate(AppWindow& app, WPARAM wParam, LPARAM lParam) {
    if (GetCapture() != app.hwnd()) return 1;
    const POINT p{static_cast<LONG>(GET_X_LPARAM(lParam)), static_cast<LONG>(GET_Y_LPARAM(lParam))};
    app.Ink().Move(p, PointerPressure(wParam));
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandlePointerUp(AppWindow& app, WPARAM wParam, LPARAM lParam) {
    if (GetCapture() != app.hwnd()) return 1;
    const POINT p{static_cast<LONG>(GET_X_LPARAM(lParam)), static_cast<LONG>(GET_Y_LPARAM(lParam))};
    app.Ink().End(p, PointerPressure(wParam));
    ReleaseCapture();
    app.Refresh();
    return 0;
}
