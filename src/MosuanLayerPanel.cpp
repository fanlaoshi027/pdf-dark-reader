#include "MosuanLayerPanel.h"
#include "AppWindow.h"
#include <algorithm>

namespace {
constexpr int kActionClose = 1;
constexpr int kActionAdd = 2;
constexpr int kActionSelect = 3;
constexpr int kActionTogglePdf = 4;
constexpr int kActionToggleBackground = 5;
}

bool MosuanLayerPanel::Paint(AppWindow& app, HDC hdc, const RECT& client) {
    const int left = (std::max)(0, client.right - kWidth);
    RECT panel{left, 0, client.right, client.bottom};
    HBRUSH bg = CreateSolidBrush(RGB(25, 27, 32));
    FillRect(hdc, &panel, bg); DeleteObject(bg);

    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(225, 228, 235));
    RECT title{left + 18, 14, client.right - 42, 50};
    DrawTextW(hdc, L"图层", -1, &title, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT close{client.right - 38, 10, client.right - 12, 42};
    DrawTextW(hdc, L"×", -1, &close, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    auto drawRow = [&](int y, const wchar_t* name, bool active, bool visible) {
        RECT r{left + 10, y, client.right - 10, y + 44};
        HBRUSH b = CreateSolidBrush(active ? RGB(52, 57, 67) : RGB(31, 34, 40));
        FillRect(hdc, &r, b); DeleteObject(b);
        SetTextColor(hdc, active ? RGB(245, 247, 250) : RGB(190, 194, 202));
        RECT text{r.left + 40, r.top, r.right - 10, r.bottom};
        DrawTextW(hdc, name, -1, &text, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SetTextColor(hdc, visible ? RGB(150, 205, 255) : RGB(80, 84, 92));
        RECT eye{r.left + 10, r.top, r.left + 32, r.bottom};
        DrawTextW(hdc, visible ? L"●" : L"○", -1, &eye, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    };

    drawRow(62, L"背景层", false, app.layers().BackgroundVisible());
    drawRow(110, L"PDF", false, app.layers().PdfVisible());
    drawRow(158, L"笔记", true, app.layers().MosuanVisible());

    RECT add{left + 10, 218, client.right - 10, 260};
    HBRUSH addBrush = CreateSolidBrush(RGB(34, 38, 45));
    FillRect(hdc, &add, addBrush); DeleteObject(addBrush);
    SetTextColor(hdc, RGB(180, 220, 255));
    DrawTextW(hdc, L"＋  新建笔记层", -1, &add, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    return true;
}

bool MosuanLayerPanel::HitTest(const RECT& client, int x, int y, int& action, std::size_t& layerIndex) {
    const int left = client.right - kWidth;
    if (x < left || x >= client.right) return false;
    layerIndex = 0; action = 0;
    if (y < 52) { action = kActionClose; return true; }
    if (y >= 62 && y < 106) { action = kActionToggleBackground; return true; }
    if (y >= 110 && y < 154) { action = kActionTogglePdf; return true; }
    if (y >= 218 && y < 260) { action = kActionAdd; return true; }
    if (y >= 158 && y < 202) { action = kActionSelect; return true; }
    return false;
}

void MosuanLayerPanel::Execute(AppWindow& app, int action, std::size_t layerIndex) {
    switch (action) {
    case kActionClose: app.SetLayerPanelOpen(false); break;
    case kActionToggleBackground: app.layers().SetBackgroundVisible(!app.layers().BackgroundVisible()); break;
    case kActionTogglePdf: app.layers().SetPdfVisible(!app.layers().PdfVisible()); break;
    case kActionSelect: app.layers().SetActiveLayer(app.layers().MosuanLayerId()); app.ApplyInkState(); break;
    case kActionAdd: app.layers().CreateNoteLayer(L"笔记层"); app.layers().SetActiveLayer(app.layers().ActiveLayerId()); app.ApplyInkState(); break;
    default: break;
    }
    app.Refresh();
}
