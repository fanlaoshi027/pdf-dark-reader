#include "AppWindowInput.h"
#include "AppWindow.h"
#include "MosuanFavoritesRail.h"
#include <windowsx.h>

LRESULT AppWindowInput::HandleLButtonDown(AppWindow& app, int x, int y, WPARAM) {
    RECT client{}; GetClientRect(app.hwnd(), &client);
    std::size_t slot = 0; bool save = false;
    if (MosuanFavoritesRail::HitTest(client, x, y, slot, save)) {
        MosuanFavoritesRail::Activate(app, slot, save);
        return 0;
    }
    if (!app.pdf().IsOpen()) return 0;
    if (app.activeTool() == MosuanTool::None) return 0;
    SetCapture(app.hwnd());
    app.ApplyInkState();
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
