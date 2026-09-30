#pragma once
#include <windows.h>
class AppWindow;
class AppWindowInput {
public:
    static LRESULT HandleKey(AppWindow& app, WPARAM key);
    static LRESULT HandleMouseWheel(AppWindow& app, WPARAM wParam, LPARAM lParam);
    static LRESULT HandleVScroll(AppWindow& app, WPARAM wParam);
    static LRESULT HandleLButtonDown(AppWindow& app, int x, int y, WPARAM flags);
    static LRESULT HandleMouseMove(AppWindow& app, int x, int y, WPARAM flags);
    static LRESULT HandleLButtonUp(AppWindow& app, int x, int y, WPARAM flags);
    static LRESULT HandlePointerDown(AppWindow& app, WPARAM wParam, LPARAM lParam);
    static LRESULT HandlePointerUpdate(AppWindow& app, WPARAM wParam, LPARAM lParam);
    static LRESULT HandlePointerUp(AppWindow& app, WPARAM wParam, LPARAM lParam);
};
