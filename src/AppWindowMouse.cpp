#include "AppWindowInput.h"
#include "AppWindow.h"
#include <windowsx.h>

LRESULT AppWindowInput::HandleLButtonDown(AppWindow& app, int x, int y, WPARAM) {
    if (!app.pdf().IsOpen()) return 0;
    SetCapture(app.hwnd());
    app.Ink().Begin(POINT{x, y}, 0.5f);
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandleMouseMove(AppWindow& app, int x, int y, WPARAM) {
    if (GetCapture() != app.hwnd()) return 0;
    app.Ink().Move(POINT{x, y}, 0.5f);
    app.Refresh();
    return 0;
}

LRESULT AppWindowInput::HandleLButtonUp(AppWindow& app, int x, int y, WPARAM) {
    if (GetCapture() != app.hwnd()) return 0;
    app.Ink().End(POINT{x, y}, 0.5f);
    ReleaseCapture();
    app.Refresh();
    return 0;
}
