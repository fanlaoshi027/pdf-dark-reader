#include "MosuanLayerPanelMessageBridge.h"

MosuanLayerPanelMessageBridge::MosuanLayerPanelMessageBridge() = default;

bool MosuanLayerPanelMessageBridge::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        return HandleMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    case WM_MOUSEMOVE:
        return HandleMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    case WM_LBUTTONUP:
        return HandleMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    case WM_LBUTTONDBLCLK:
        return HandleDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    default:
        return false;
    }
}

bool MosuanLayerPanelMessageBridge::HandleMouseDown(int x, int y)
{
    if (!router_) return false;
    return router_->OnMouseDown({x, y});
}

bool MosuanLayerPanelMessageBridge::HandleMouseMove(int x, int y)
{
    if (!router_) return false;
    return router_->OnMouseMove({x, y});
}

bool MosuanLayerPanelMessageBridge::HandleMouseUp(int x, int y)
{
    if (!router_) return false;
    return router_->OnMouseUp({x, y});
}

bool MosuanLayerPanelMessageBridge::HandleDoubleClick(int x, int y)
{
    if (!router_) return false;
    return router_->OnDoubleClick({x, y});
}
