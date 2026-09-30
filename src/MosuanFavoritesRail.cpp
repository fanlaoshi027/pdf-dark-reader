#include "MosuanFavoritesRail.h"
#include "AppWindow.h"
#include <algorithm>

namespace {
COLORREF BrushColor(const BrushState& state) {
    switch (state.color) {
    case InkColor::Red: return RGB(220, 55, 55);
    case InkColor::Blue: return RGB(55, 105, 225);
    default: return RGB(235, 235, 235);
    }
}

int BrushWidth(const BrushState& state) {
    switch (state.width) {
    case InkWidth::Medium: return 3;
    case InkWidth::Thick: return 5;
    default: return 2;
    }
}
}

void MosuanFavoritesRail::Paint(AppWindow& app, HDC hdc, const RECT& client) {
    const int left = client.right - kWidth;
    RECT rail{left, 0, client.right, client.bottom};
    HBRUSH bg = CreateSolidBrush(RGB(22, 24, 29));
    FillRect(hdc, &rail, bg);
    DeleteObject(bg);

    HPEN border = CreatePen(PS_SOLID, 1, RGB(55, 58, 65));
    HGDIOBJ oldPen = SelectObject(hdc, border);
    MoveToEx(hdc, left, 0, nullptr); LineTo(hdc, left, client.bottom);
    SelectObject(hdc, oldPen); DeleteObject(border);

    const auto& favorites = app.Mosuan().Favorites();
    for (std::size_t i = 0; i < FavoriteToolStore::kMaxSlots; ++i) {
        const int top = 8 + static_cast<int>(i) * kSlotHeight;
        RECT r{left + 5, top, client.right - 5, top + kSlotHeight - 5};
        const FavoriteTool* favorite = favorites.Get(i);
        const bool occupied = favorite != nullptr && favorite->occupied;

        HBRUSH slotBrush = CreateSolidBrush(RGB(28, 31, 37));
        FillRect(hdc, &r, slotBrush);
        DeleteObject(slotBrush);

        if (occupied) {
            const int cy = (r.top + r.bottom) / 2;
            HPEN pen = CreatePen(favorite->state.dashed ? PS_DASH : PS_SOLID,
                                 BrushWidth(favorite->state), BrushColor(favorite->state));
            HGDIOBJ old = SelectObject(hdc, pen);
            MoveToEx(hdc, r.left + 9, cy, nullptr);
            LineTo(hdc, r.right - 9, cy);
            SelectObject(hdc, old);
            DeleteObject(pen);
        } else {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(90, 94, 102));
            DrawTextW(hdc, L"+", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    RECT layerButton{left + 5, client.bottom - 50, client.right - 5, client.bottom - 8};
    HBRUSH layerBrush = CreateSolidBrush(RGB(31, 34, 40));
    FillRect(hdc, &layerButton, layerBrush); DeleteObject(layerBrush);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(205, 210, 218));
    DrawTextW(hdc, L"≡", -1, &layerButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

bool MosuanFavoritesRail::HitTest(const RECT& client, int x, int y, std::size_t& slot, bool& save) {
    const int left = client.right - kWidth;
    if (x < left || x >= client.right) return false;
    if (y >= client.bottom - 56 || y < 8) return false;
    const int index = (y - 8) / kSlotHeight;
    if (index < 0 || index >= static_cast<int>(FavoriteToolStore::kMaxSlots)) return false;
    slot = static_cast<std::size_t>(index);
    save = false;
    return true;
}

void MosuanFavoritesRail::Activate(AppWindow& app, std::size_t slot, bool save) {
    if (slot >= FavoriteToolStore::kMaxSlots) return;
    if (save) {
        app.SaveInkSlot(slot);
        app.Refresh();
        return;
    }
    const FavoriteTool* favorite = app.Mosuan().Favorites().Get(slot);
    if (!favorite || !favorite->occupied) app.SaveInkSlot(slot);
    else app.LoadInkSlot(slot);
    app.Refresh();
}
