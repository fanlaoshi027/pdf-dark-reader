#include "AppWindowInput.h"
#include "AppWindow.h"
#include "AppWindowPdf.h"
#include "MosuanFavoritesRail.h"
#include <windowsx.h>
#include <algorithm>

namespace {
constexpr int kToolbarHeight = 70;

bool InPage(const AppWindow& app, int x, int y) {
    if (!app.pdf().IsOpen()) return false;
    if (y < kToolbarHeight) return false;
    RECT rc{};
    GetClientRect(app.hwnd(), &rc);
    if (x < 0 || x >= rc.right - MosuanFavoritesRail::kWidth || y >= rc.bottom) return false;
    return true;
}

float PointerPressure(WPARAM wParam) {
    const UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
    POINTER_TYPE type = PT_POINTER;
    if (GetPointerType(pointerId, &type) && type == PT_PEN) {
        POINTER_PEN_INFO pen{};
        if (GetPointerPenInfo(pointerId, &pen)) {
            // Windows Ink pressure is normalized to [0, 1024].
            return static_cast<float>(pen.pressure) / 1024.0f;
        }
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

LRESULT AppWindowInput::HandleLButtonDown(AppWindow& app, int x, int y, WPARAM flags) {
    RECT client{}; GetClientRect(app.hwnd(), &client);
    std::size_t slot = 0; bool save = false;
    if (MosuanFavoritesRail::HitTest(client, x, y, slot, save)) {
        MosuanFavoritesRail::Activate(app, slot, save);
        return 0;
    }
    if (!InPage(app, x, y)) return 1;

    SetCapture(app.hwnd());
    POINT p{x, y};
    if (app.activeTool() == MosuanTool::Ruler) {
        ReleaseCapture();
        return 0;
    }
    app.Ink().Begin(p, 0.5f);
    app.Refresh();
    (void)flags;
    return 0;
}

LRESULT AppWindowInput::HandleMouseMove(AppWindow& app, int x, int y, WPARAM flags) {
    if (!app.Ink().IsDrawing()) return 1;
    POINT p{x, y};
    app.Ink().Move(p, 0.5f);
    app.Refresh();
    (void)flags;
    return 0;
}

LRESULT AppWindowInput::HandleLButtonUp(AppWindow& app, int x, int y, WPARAM flags) {
    if (!app.Ink().IsDrawing()) return 1;
    POINT p{x, y};
    app.Ink().End(p, 0.5f);
    ReleaseCapture();
    app.Refresh();
    (void)flags;
    return 0;
}

LRESULT AppWindowInput::HandlePointerDown(AppWindow& app, WPARAM wParam, LPARAM lParam) {
    const POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    if (!InPage(app, p.x, p.y)) return 1;
    SetCapture(app.hwnd());
    app.Ink().Begin(p, PointerPressure(wParam));
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandlePointerUpdate(AppWindow& app, WPARAM wParam, LPARAM lParam) {
    if (!app.Ink().IsDrawing()) return 1;
    const POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    app.Ink().Move(p, PointerPressure(wParam));
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandlePointerUp(AppWindow& app, WPARAM wParam, LPARAM lParam) {
    if (!app.Ink().IsDrawing()) return 1;
    const POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    app.Ink().End(p, PointerPressure(wParam));
    ReleaseCapture();
    app.Refresh();
    return 0;
}
