#pragma once
#include <windows.h>
#include <cstddef>

class AppWindow;

class MosuanLayerPanel {
public:
    static constexpr int kWidth = 280;
    static bool Paint(AppWindow& app, HDC hdc, const RECT& client);
    static bool HitTest(const RECT& client, int x, int y, int& action, std::size_t& layerIndex);
    static void Execute(AppWindow& app, int action, std::size_t layerIndex);

    // 图层编辑交互
    static void BeginRename(AppWindow& app, std::size_t layerIndex);
    static void CommitRename(AppWindow& app);
    static void CancelRename(AppWindow& app);
    static void BeginDrag(AppWindow& app, std::size_t layerIndex, int y);
    static void UpdateDrag(AppWindow& app, int y);
    static void EndDrag(AppWindow& app);
};
