#include "AppWindowInput.h"
#include "AppWindow.h"
#include "AppWindowPdf.h"
#include <windowsx.h>

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

LRESULT AppWindowInput::HandlePointerDown(AppWindow&, WPARAM, LPARAM) { return 1; }
LRESULT AppWindowInput::HandlePointerUpdate(AppWindow&, WPARAM, LPARAM) { return 1; }
LRESULT AppWindowInput::HandlePointerUp(AppWindow&, WPARAM, LPARAM) { return 1; }
