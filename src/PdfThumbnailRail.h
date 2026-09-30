#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>

class AppWindow;

class PdfThumbnailRail {
public:
    static constexpr int kWidth = 112;
    static constexpr int kTop = 58;
    static void Paint(AppWindow& app, HDC hdc, const RECT& client);
    static bool HitTest(AppWindow& app, int x, int y, int& page);
    static bool Contains(int x, int y, const RECT& client);
    static void Scroll(AppWindow& app, int delta);
    static void ResetCache();
};
