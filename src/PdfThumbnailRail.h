#pragma once
#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <vector>

class AppWindow;

class PdfThumbnailRail {
public:
    static constexpr int kWidth = 112;
    static constexpr int kTop = 58;
    static void Install();
    static void Ensure(HWND parent);
    static void Resize(HWND parent);
    static void Refresh(HWND parent);
    static void ResetCache();
};
