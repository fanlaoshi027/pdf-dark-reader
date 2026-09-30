#pragma once

#include <windows.h>

class MosuanLayerPanelController;

class MosuanLayerPanelMessageBridge {
public:
    void Attach(MosuanLayerPanelController* controller) { controller_ = controller; }
    bool HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    MosuanLayerPanelController* controller_ = nullptr;
};
