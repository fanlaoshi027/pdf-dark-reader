#pragma once

#include <windows.h>
#include <vector>

// Windows Ink bridge for the Win32 editor.
// The first implementation keeps the existing PDF/layer architecture intact
// and exposes a small boundary for native pen input. Rendering/storage can be
// switched independently after the input path is verified on real hardware.
class WindowsInkBridge {
public:
    WindowsInkBridge() = default;
    ~WindowsInkBridge() = default;

    bool Initialize(HWND hostWindow);
    void Shutdown();

    bool IsInitialized() const { return initialized_; }
    HWND HostWindow() const { return hostWindow_; }

    // Returns true when the bridge consumed a pointer message.
    bool HandlePointerMessage(UINT message, WPARAM wParam, LPARAM lParam);

    // Clear the current in-memory stroke preview.
    void ClearPreview();

private:
    HWND hostWindow_ = nullptr;
    bool initialized_ = false;
    bool penInContact_ = false;
    std::vector<POINT> previewPoints_;
};
