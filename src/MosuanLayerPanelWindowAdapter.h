#pragma once

#include <windows.h>

class MosuanLayerPanelMessageBridge;

class MosuanLayerPanelWindowAdapter {
public:
    void AttachBridge(MosuanLayerPanelMessageBridge* bridge);
    bool HandleWindowMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    MosuanLayerPanelMessageBridge* bridge_ = nullptr;
};
