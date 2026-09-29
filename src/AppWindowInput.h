#pragma once

#include <windows.h>

class AppWindow;

class AppWindowInput {
public:
    static LRESULT HandleKey(AppWindow& app, WPARAM key);
    static LRESULT HandleMouseWheel(AppWindow& app, WPARAM wParam);
    static LRESULT HandleVScroll(AppWindow& app, WPARAM wParam);
};
