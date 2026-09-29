#pragma once
#include <windows.h>
class AppWindow;
class AppWindowInput {
public:
    static LRESULT HandleKey(AppWindow& app, WPARAM key);
    static LRESULT HandleMouseWheel(AppWindow& app, WPARAM wParam);
    static LRESULT HandleVScroll(AppWindow& app, WPARAM wParam);
    static LRESULT HandleLButtonDown(AppWindow& app, int x, int y, WPARAM flags);
    static LRESULT HandleMouseMove(AppWindow& app, int x, int y, WPARAM flags);
    static LRESULT HandleLButtonUp(AppWindow& app, int x, int y, WPARAM flags);
};
