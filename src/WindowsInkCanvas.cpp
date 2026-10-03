#include "WindowsInkCanvas.h"
#include <algorithm>
#include <windowsx.h>

namespace {
constexpr wchar_t kInkCanvasClass[] = L"MosuanWindowsInkCanvas";
constexpr wchar_t kAppClassName[] = L"PDFDarkReaderWindow";
constexpr UINT_PTR kLayoutTimer = 17;
constexpr int kToolbarHeight = 58;
constexpr int kThumbnailRailWidth = 112;

WindowsInkCanvas g_canvas;
HHOOK g_hook = nullptr;

void LayoutCanvas(HWND parent) {
    if (!parent || !IsWindow(parent) || !g_canvas.IsReady()) return;
    RECT rc{};
    GetClientRect(parent, &rc);
    RECT bounds{kThumbnailRailWidth, kToolbarHeight,
        (std::max)(kThumbnailRailWidth + 1, (int)rc.right),
        (std::max)(kToolbarHeight + 1, (int)rc.bottom)};
    g_canvas.Resize(bounds);
}

LRESULT CALLBACK CanvasCbtProc(int code, WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    if (code == HCBT_ACTIVATE) {
        HWND hwnd = reinterpret_cast<HWND>(wParam);
        wchar_t cls[128]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        if (lstrcmpW(cls, kAppClassName) == 0) {
            if (!g_canvas.IsReady()) {
                g_canvas.Create(hwnd);
                g_canvas.SetVisible(true);
            }
            LayoutCanvas(hwnd);
        }
    }
    return CallNextHookEx(g_hook, code, wParam, lParam);
}

struct CanvasHookInstaller {
    CanvasHookInstaller() { g_hook = SetWindowsHookExW(WH_CBT, CanvasCbtProc, nullptr, GetCurrentThreadId()); }
    ~CanvasHookInstaller() { if (g_hook) UnhookWindowsHookEx(g_hook); g_hook = nullptr; }
} g_hookInstaller;
}

WindowsInkCanvas::~WindowsInkCanvas() { Shutdown(); }

bool WindowsInkCanvas::Create(HWND parent) {
    Shutdown();
    if (!parent || !IsWindow(parent)) return false;
    parent_ = parent;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(parent, GWLP_HINSTANCE));
    wc.lpfnWndProc = &WindowsInkCanvas::CanvasProc;
    wc.lpszClassName = kInkCanvasClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) { parent_ = nullptr; return false; }
    hwnd_ = CreateWindowExW(WS_EX_NOACTIVATE, kInkCanvasClass, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 1, 1, parent_, nullptr, wc.hInstance, this);
    if (!hwnd_) { parent_ = nullptr; return false; }
    ready_ = ink_.Initialize(hwnd_);
    if (!ready_) { DestroyWindow(hwnd_); hwnd_ = nullptr; parent_ = nullptr; return false; }
    SetTimer(hwnd_, kLayoutTimer, 100, nullptr);
    return true;
}

void WindowsInkCanvas::Resize(const RECT& bounds) {
    if (!hwnd_) return;
    LONG width = bounds.right - bounds.left;
    LONG height = bounds.bottom - bounds.top;
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    MoveWindow(hwnd_, bounds.left, bounds.top, width, height, TRUE);
    ink_.Resize();
}

void WindowsInkCanvas::SetVisible(bool visible) {
    if (!hwnd_) return;
    ShowWindow(hwnd_, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
    ink_.SetEnabled(visible);
}

void WindowsInkCanvas::Clear() { if (ready_) ink_.Clear(); }

void WindowsInkCanvas::Shutdown() {
    if (hwnd_) KillTimer(hwnd_, kLayoutTimer);
    ink_.Shutdown();
    ready_ = false;
    if (hwnd_) DestroyWindow(hwnd_);
    hwnd_ = nullptr;
    parent_ = nullptr;
}

LRESULT CALLBACK WindowsInkCanvas::CanvasProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<WindowsInkCanvas*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<WindowsInkCanvas*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    return self ? self->HandleMessage(message, wParam, lParam) : DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT WindowsInkCanvas::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    switch (message) {
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_NCHITTEST: return HTCLIENT;
    case WM_TIMER: if (wParam == kLayoutTimer) LayoutCanvas(parent_); return 0;
    case WM_SIZE: if (ready_) ink_.Resize(); return 0;
    case WM_DESTROY: ink_.Shutdown(); ready_ = false; return 0;
    default: return DefWindowProcW(hwnd_, message, wParam, lParam);
    }
}
