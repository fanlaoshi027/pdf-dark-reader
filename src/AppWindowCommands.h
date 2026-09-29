#pragma once

#include <windows.h>

class AppWindow;

class AppWindowCommands {
public:
    static LRESULT Execute(AppWindow& app, int commandId);
};
