#include "WindowsInkHost.h"

#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <d3d11.h>
#include <dcomp.h>
#include <dxgi1_2.h>
#include <inkpresenterdesktop.h>

using Microsoft::WRL::ComPtr;

namespace {

HRESULT CreateInkD3DDevice(
    D3D_DRIVER_TYPE driverType,
    UINT flags,
    ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context) {
    const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    D3D_FEATURE_LEVEL level{};
    return D3D11CreateDevice(
        nullptr,
        driverType,
        nullptr,
        flags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        &device,
        &level,
        &context);
}

}

WindowsInkHost::~WindowsInkHost() {
    Shutdown();
}

bool WindowsInkHost::Initialize(HWND target) {
    Shutdown();
    if (!target || !IsWindow(target)) return false;
    target_ = target;

    const UINT baseFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = E_FAIL;

    ComPtr<ID3D11Device> d3dDevice;
    ComPtr<ID3D11DeviceContext> d3dContext;

#ifdef _DEBUG
    hr = CreateInkD3DDevice(D3D_DRIVER_TYPE_HARDWARE,
                            baseFlags | D3D11_CREATE_DEVICE_DEBUG,
                            d3dDevice, d3dContext);
    if (FAILED(hr)) {
        d3dDevice.Reset();
        d3dContext.Reset();
        hr = CreateInkD3DDevice(D3D_DRIVER_TYPE_HARDWARE,
                                baseFlags, d3dDevice, d3dContext);
    }
#else
    hr = CreateInkD3DDevice(D3D_DRIVER_TYPE_HARDWARE,
                            baseFlags, d3dDevice, d3dContext);
#endif

    if (FAILED(hr)) {
        d3dDevice.Reset();
        d3dContext.Reset();
        hr = CreateInkD3DDevice(D3D_DRIVER_TYPE_WARP,
                                baseFlags, d3dDevice, d3dContext);
    }
    if (FAILED(hr)) return false;

    ComPtr<IDXGIDevice> dxgiDevice;
    if (FAILED(d3dDevice.As(&dxgiDevice))) return false;

    ComPtr<IDCompositionDevice> dcompDevice;
    if (FAILED(DCompositionCreateDevice(
            dxgiDevice.Get(), IID_PPV_ARGS(&dcompDevice)))) return false;

    ComPtr<IDCompositionTarget> dcompTarget;
    if (FAILED(dcompDevice->CreateTargetForHwnd(
            target_, TRUE, &dcompTarget))) return false;

    ComPtr<IDCompositionVisual> root;
    if (FAILED(dcompDevice->CreateVisual(&root))) return false;
    if (FAILED(dcompTarget->SetRoot(root.Get()))) return false;

    ComPtr<IInkDesktopHost> desktopHost;
    hr = CoCreateInstance(__uuidof(InkDesktopHost), nullptr,
                          CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&desktopHost));
    if (FAILED(hr)) return false;

    RECT rc{};
    GetClientRect(target_, &rc);
    const float width = static_cast<float>((std::max)(1L, rc.right - rc.left));
    const float height = static_cast<float>((std::max)(1L, rc.bottom - rc.top));

    ComPtr<IInkPresenterDesktop> presenter;
    hr = desktopHost->CreateAndInitializeInkPresenter(
        root.Get(), width, height, IID_PPV_ARGS(&presenter));
    if (FAILED(hr)) return false;

    if (FAILED(dcompDevice->Commit())) return false;

    d3dDevice_ = d3dDevice;
    d3dContext_ = d3dContext;
    dcompDevice_ = dcompDevice;
    dcompTarget_ = dcompTarget;
    dcompRoot_ = root;
    desktopHost_ = desktopHost;
    presenter_ = presenter;
    initialized_ = true;
    SetEnabled(true);
    return true;
}

bool WindowsInkHost::Resize() {
    if (!initialized_ || !presenter_ || !dcompDevice_ || !target_) return false;

    RECT rc{};
    GetClientRect(target_, &rc);
    const float width = static_cast<float>((std::max)(1L, rc.right - rc.left));
    const float height = static_cast<float>((std::max)(1L, rc.bottom - rc.top));

    if (FAILED(presenter_->SetSize(width, height))) return false;
    return SUCCEEDED(dcompDevice_->Commit());
}

void WindowsInkHost::SetEnabled(bool enabled) {
    if (!initialized_ || !dcompRoot_ || !dcompDevice_) return;
    // IDCompositionVisual3 owns SetOpacity in current Windows SDKs. The
    // native InkPresenter itself remains attached to the visual tree here;
    // keep this state change independent of SDK-specific visual extensions.
    enabled_ = enabled;
    dcompDevice_->Commit();
}

void WindowsInkHost::Clear() {
    if (!target_ || !IsWindow(target_)) return;
    const bool wasEnabled = enabled_;
    const HWND target = target_;
    Initialize(target);
    SetEnabled(wasEnabled);
}

void WindowsInkHost::Shutdown() {
    enabled_ = false;
    presenter_.Reset();
    desktopHost_.Reset();
    dcompTarget_.Reset();
    dcompRoot_.Reset();
    if (dcompDevice_) dcompDevice_->WaitForCommitCompletion();
    dcompDevice_.Reset();
    d3dContext_.Reset();
    d3dDevice_.Reset();
    target_ = nullptr;
    initialized_ = false;
}
