#pragma once
#include <windows.h>
#include <cstddef>

class AppWindow;

class MosuanFavoritesRail {
public:
    static constexpr int kWidth = 46;
    static constexpr int kSlotHeight = 52;
    static void Paint(AppWindow& app, HDC hdc, const RECT& client);
    static bool HitTest(const RECT& client, int x, int y, std::size_t& slot, bool& save);
    static void Activate(AppWindow& app, std::size_t slot, bool save);
};
