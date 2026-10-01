#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dcomp.h>
#include <inkpresenterdesktop.h>
#include <wrl/client.h>

// Native Windows Ink host for the Win32 PDF viewer.
// Ink is used for the primary Pen tool; legacy GDI+ remains available for
// tools that require application-side geometry (line, lasso, eraser).
class WindowsInkHost {
public:
    WindowsInkHost() = default;
    ~WindowsInkHost();

    WindowsInkHost(const WindowsInkHost&) = delete;
    WindowsInkHost& operator=(const WindowsInkHost&) = delete;

    bool Initialize(HWND target);
    bool Resize();
    void SetEnabled(bool enabled);
    void Clear();
    void Shutdown();
    bool IsAvailable() const { return initialized_; }
    bool IsEnabled() const { return enabled_; }

private:
    HWND target_ = nullptr;
    bool initialized_ = false;
    bool enabled_ = false;

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext_;
    Microsoft::WRL::ComPtr<IDCompositionDevice> dcompDevice_;
    Microsoft::WRL::ComPtr<IDCompositionTarget> dcompTarget_;
    Microsoft::WRL::ComPtr<IDCompositionVisual> dcompRoot_;
    Microsoft::WRL::ComPtr<IInkDesktopHost> desktopHost_;
    Microsoft::WRL::ComPtr<IInkPresenterDesktop> presenter_;
};
