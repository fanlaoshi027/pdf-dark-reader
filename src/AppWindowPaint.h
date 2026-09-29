#pragma once

#include <windows.h>

class AppWindow;

class AppWindowPaint {
public:
    static void Paint(AppWindow& app, HDC hdc);
    static void DrawToolbarButton(AppWindow& app, const DRAWITEMSTRUCT* dis);
};
