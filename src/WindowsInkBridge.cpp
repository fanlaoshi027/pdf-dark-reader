#include "WindowsInkBridge.h"

bool WindowsInkBridge::Initialize(HWND hostWindow) {
    Shutdown();
    if (!hostWindow || !IsWindow(hostWindow)) return false;
    hostWindow_ = hostWindow;
    initialized_ = true;
    penInContact_ = false;
    previewPoints_.clear();
    return true;
}

void WindowsInkBridge::Shutdown() {
    previewPoints_.clear();
    penInContact_ = false;
    initialized_ = false;
    hostWindow_ = nullptr;
}

bool WindowsInkBridge::HandlePointerMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    if (!initialized_) return false;

    switch (message) {
    case WM_POINTERDOWN:
        penInContact_ = true;
        previewPoints_.clear();
        previewPoints_.push_back(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        return false;

    case WM_POINTERUPDATE:
        if (!penInContact_) return false;
        previewPoints_.push_back(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        return false;

    case WM_POINTERUP:
        if (!penInContact_) return false;
        penInContact_ = false;
        previewPoints_.push_back(POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        return false;
    }

    (void)wParam;
    return false;
}

void WindowsInkBridge::ClearPreview() {
    previewPoints_.clear();
    penInContact_ = false;
}
