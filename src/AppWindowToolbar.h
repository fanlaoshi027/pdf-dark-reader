#pragma once

#include <windows.h>

class AppWindow;

class AppWindowToolbar {
public:
    static void Create(AppWindow& app);
    static void Layout(AppWindow& app, int width);
    static void UpdateText(AppWindow& app);
};
