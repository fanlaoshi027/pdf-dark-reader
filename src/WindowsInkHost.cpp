#include "WindowsInkHost.h"

#define NOMINMAX
#include <windows.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dxgi1_2.h>
#include <inkpresenterdesktop.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace {

bool Succeeded(HRESULT hr) {
    return SUCCEEDED(hr);
}

}

WindowsInkHost::~WindowsInkHost() {
    Shutdown();
}

bool WindowsInkHost::Initialize(HWND target) {
    Shutdown();
    if (!target || !IsWindow(target)) return false;

    target_ = target;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL level{};
    const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    ComPtr<ID3D11Device> d3dDevice;
    ComPtr<ID3D11DeviceContext> d3dContext;
    if (!Succeeded(D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            &d3dDevice,
            &level,
            &d3dContext))) {
        return false;
    }

    ComPtr<IDXGIDevice> dxgiDevice;
    if (!Succeeded(d3dDevice.As(&dxgiDevice))) return false;

    ComPtr<IDCompositionDevice> dcompDevice;
    if (!Succeeded(DCompositionCreateDevice(
            dxgiDevice.Get(),
            IID_PPV_ARGS(&dcompDevice)))) {
        return false;
    }

    ComPtr<IDCompositionTarget> target;
    if (!Succeeded(dcompDevice->CreateTargetForHwnd(target_, TRUE, &target))) {
        return false;
    }

    ComPtr<IDCompositionVisual> root;
    if (!Succeeded(dcompDevice->CreateVisual(&root))) return false;
    if (!Succeeded(target->SetRoot(root.Get()))) return false;

    ComPtr<IInkDesktopHost> desktopHost;
    HRESULT hr = CoCreateInstance(
        __uuidof(InkDesktopHost),
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&desktopHost));
    if (!Succeeded(hr)) return false;

    RECT rc{};
    GetClientRect(target_, &rc);
    const float width = static_cast<float>((std::max)(1L, rc.right - rc.left));
    const float height = static_cast<float>((std::max)(1L, rc.bottom - rc.top));

    ComPtr<IInkPresenterDesktop> presenter;
    hr = desktopHost->CreateAndInitializeInkPresenter(
        root.Get(),
        width,
        height,
        IID_PPV_ARGS(&presenter));
    if (!Succeeded(hr)) return false;

    if (!Succeeded(dcompDevice->Commit())) return false;

    d3dDevice_ = d3dDevice;
    d3dContext_ = d3dContext;
    dcompDevice_ = dcompDevice;
    dcompTarget_ = target;
    dcompRoot_ = root;
    desktopHost_ = desktopHost;
    presenter_ = presenter;
    initialized_ = true;
    return true;
}

void WindowsInkHost::Shutdown() {
    presenter_.Reset();
    desktopHost_.Reset();
    if (dcompDevice_) dcompDevice_->WaitForCommitCompletion();
    dcompTarget_.Reset();
    dcompRoot_.Reset();
    dcompDevice_.Reset();
    d3dContext_.Reset();
    d3dDevice_.Reset();
    target_ = nullptr;
    initialized_ = false;
}

bool WindowsInkHost::Resize() {
    if (!initialized_ || !presenter_ || !target_) return false;
    RECT rc{};
    GetClientRect(target_, &rc);
    const float width = static_cast<float>((std::max)(1L, rc.right - rc.left));
    const float height = static_cast<float>((std::max)(1L, rc.bottom - rc.top));
    const HRESULT hr = presenter_->SetSize(width, height);
    if (FAILED(hr)) return false;
    return dcompDevice_ ? SUCCEEDED(dcompDevice_->Commit()) : false;
}
