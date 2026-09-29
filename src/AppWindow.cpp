#include "AppWindow.h"
#include "AppWindowCommands.h"
#include "AppWindowInput.h"
#include "AppWindowPaint.h"
#include "AppWindowPdf.h"
#include "AppWindowToolbar.h"
#include <algorithm>

namespace {
constexpr wchar_t kClassName[] = L"PDFDarkReaderWindow";
constexpr wchar_t kTitle[] = L"Mosuan 墨算";
constexpr int kToolbarHeight = 70;
}

bool AppWindow::Create(HINSTANCE instance) {
    instance_ = instance;
    invertSettings_.strength = 0.90f;
    invertSettings_.backgroundR = invertSettings_.backgroundG = invertSettings_.backgroundB = 26;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = &AppWindow::WindowProc;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

    hwnd_ = CreateWindowExW(0, kClassName, kTitle,
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 850,
        nullptr, nullptr, instance, this);
    if (!hwnd_) return false;

    layers_.Create(hwnd_);
    layers_.SetTool(MosuanTool::Pen);
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
    return true;
}

int AppWindow::Run() {
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK AppWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<AppWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<AppWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->hwnd_ = hwnd;
    }
    return self ? self->HandleMessage(message, wParam, lParam)
                : DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT AppWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND: {
        const LRESULT result = AppWindowCommands::Execute(*this, LOWORD(wParam));
        if (result == 0) return 0;
        break;
    }
    case WM_KEYDOWN:
        if (AppWindowInput::HandleKey(*this, wParam) == 0) return 0;
        break;
    case WM_MOUSEWHEEL:
        return AppWindowInput::HandleMouseWheel(*this, wParam);
    case WM_VSCROLL:
        return AppWindowInput::HandleVScroll(*this, wParam);
    case WM_SIZE:
        if (pdf_.IsOpen()) AppWindowPdf::Render(*this);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd_, &ps);
        AppWindowPaint::Paint(*this, hdc);
        EndPaint(hwnd_, &ps);
        return 0;
    }
    case WM_MOSUAN_NOTE_VIS:
    case WM_MOSUAN_BG_VIS:
        Refresh();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd_, message, wParam, lParam);
}

void AppWindow::UpdateScrollBar() {
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight);
    const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20);
    SCROLLINFO si{};
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = maxScroll;
    si.nPage = static_cast<UINT>(viewportH);
    si.nPos = std::clamp(scrollY_, 0, maxScroll);
    SetScrollInfo(hwnd_, SB_VERT, &si, TRUE);
}

void AppWindow::UpdateToolbarText() {
    // The icon-only toolbar is rendered by MosuanUi.
}
