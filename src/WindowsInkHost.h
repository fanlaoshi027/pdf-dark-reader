#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dcomp.h>
#include <inkpresenterdesktop.h>
#include <wrl/client.h>

// Native Windows Ink host for the Win32 PDF viewer.
// The first integration stage hosts InkPresenter beside the existing
// Pointer/GDI+ path so the legacy renderer can remain as a safe fallback.
class WindowsInkHost {
public:
    WindowsInkHost() = default;
    ~WindowsInkHost();

    WindowsInkHost(const WindowsInkHost&) = delete;
    WindowsInkHost& operator=(const WindowsInkHost&) = delete;

    bool Initialize(HWND target);
    bool Resize();
    void Shutdown();
    bool IsAvailable() const { return initialized_; }

private:
    HWND target_ = nullptr;
    bool initialized_ = false;

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext_;
    Microsoft::WRL::ComPtr<IDCompositionDevice> dcompDevice_;
    Microsoft::WRL::ComPtr<IDCompositionTarget> dcompTarget_;
    Microsoft::WRL::ComPtr<IDCompositionVisual> dcompRoot_;
    Microsoft::WRL::ComPtr<IInkDesktopHost> desktopHost_;
    Microsoft::WRL::ComPtr<IInkPresenterDesktop> presenter_;
};
