#pragma once

#include <windows.h>

class AppWindow {
public:
    bool Create(HINSTANCE instance);
    int Run();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    void Paint(HDC hdc);

    HWND hwnd_ = nullptr;
    bool invert_ = false;
};
