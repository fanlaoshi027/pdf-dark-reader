#pragma once

#include <windows.h>

// Windows Ink bridge for the Win32 PDF viewer.
// This first stage deliberately keeps the existing Pointer/GDI+ path intact.
// The host owns the native InkDesktopHost/InkPresenter objects once enabled,
// while LayerSystem remains the fallback renderer.
class WindowsInkHost {
public:
    WindowsInkHost() = default;
    ~WindowsInkHost();

    WindowsInkHost(const WindowsInkHost&) = delete;
    WindowsInkHost& operator=(const WindowsInkHost&) = delete;

    bool Initialize(HWND target);
    void Shutdown();
    bool IsAvailable() const { return initialized_; }

private:
    HWND target_ = nullptr;
    bool initialized_ = false;
    HMODULE inkHostModule_ = nullptr;
    void* desktopHost_ = nullptr;
    void* presenter_ = nullptr;
};
