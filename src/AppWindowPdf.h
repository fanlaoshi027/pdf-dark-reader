#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>

class AppWindow;

class AppWindowPdf {
public:
    static void Open(AppWindow& app);
    static void Render(AppWindow& app);
    static void Zoom(AppWindow& app, double factor);
    static void FitWidth(AppWindow& app);
    static void GoPage(AppWindow& app, int delta);
    static void Scroll(AppWindow& app, int delta);
};
