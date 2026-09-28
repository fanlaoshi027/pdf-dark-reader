#include "AppWindow.h"
#include <algorithm>
#include <windowsx.h>
#include <commdlg.h>

namespace {
constexpr wchar_t kClassName[] = L"PDFDarkReaderWindow";
constexpr wchar_t kTitle[] = L"PDF Dark Reader";

void InvertBgra(std::vector<std::uint8_t>& pixels, const InvertSettings& s) {
    if (!s.enabled) return;
    const float strength = std::clamp(s.strength, 0.0f, 1.0f);
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        const float b = 255.0f - pixels[i];
        const float g = 255.0f - pixels[i + 1];
        const float r = 255.0f - pixels[i + 2];
        pixels[i]     = static_cast<std::uint8_t>(s.backgroundB + (b - s.backgroundB) * strength);
        pixels[i + 1] = static_cast<std::uint8_t>(s.backgroundG + (g - s.backgroundG) * strength);
        pixels[i + 2] = static_cast<std::uint8_t>(s.backgroundR + (r - s.backgroundR) * strength);
    }
}
}

bool AppWindow::Create(HINSTANCE instance) {
    instance_ = instance;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = &AppWindow::WindowProc;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
    hwnd_ = CreateWindowExW(0, kClassName, kTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 850, nullptr, nullptr, instance, this);
    if (!hwnd_) return false;
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
    return true;
}

int AppWindow::Run() {
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
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
    return self ? self->HandleMessage(message, wParam, lParam) : DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT AppWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_KEYDOWN:
        if (wParam == 'O' && (GetKeyState(VK_CONTROL) & 0x8000)) { OpenPdf(); return 0; }
        if (wParam == 'I' && pdf_.IsOpen()) { invert_ = !invert_; RenderCurrentPage(); InvalidateRect(hwnd_, nullptr, FALSE); return 0; }
        if (wParam == VK_LEFT || wParam == VK_UP) { if (pageIndex_ > 0) { --pageIndex_; RenderCurrentPage(); InvalidateRect(hwnd_, nullptr, FALSE); } return 0; }
        if (wParam == VK_RIGHT || wParam == VK_DOWN) { if (pageIndex_ + 1 < pdf_.PageCount()) { ++pageIndex_; RenderCurrentPage(); InvalidateRect(hwnd_, nullptr, FALSE); } return 0; }
        break;
    case WM_SIZE:
        if (pdf_.IsOpen()) { RenderCurrentPage(); InvalidateRect(hwnd_, nullptr, FALSE); }
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps{}; HDC hdc = BeginPaint(hwnd_, &ps); Paint(hdc); EndPaint(hwnd_, &ps); return 0;
    }
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd_, message, wParam, lParam);
}

void AppWindow::OpenPdf() {
    OPENFILENAMEW ofn{};
    wchar_t file[MAX_PATH]{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd_;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"PDF files (*.pdf)\0*.pdf\0All files (*.*)\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameW(&ofn)) {
        if (pdf_.Open(file)) {
            pageIndex_ = 0;
            invert_ = false;
            RenderCurrentPage();
            InvalidateRect(hwnd_, nullptr, TRUE);
        } else {
            MessageBoxW(hwnd_, L"无法打开这个 PDF 文件。", L"PDF Dark Reader", MB_ICONERROR);
        }
    }
}

void AppWindow::RenderCurrentPage() {
    if (!pdf_.IsOpen()) return;
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const int availableW = (std::max)(200, static_cast<int>(rc.right - 40));
    const int availableH = (std::max)(200, static_cast<int>(rc.bottom - 80));
    float pageW = 1, pageH = 1;
    if (!pdf_.PageSize(pageIndex_, pageW, pageH)) return;
    const double scale = std::min(static_cast<double>(availableW) / pageW, static_cast<double>(availableH) / pageH);
    renderWidth_ = (std::max)(1, static_cast<int>(pageW * scale));
    renderHeight_ = (std::max)(1, static_cast<int>(pageH * scale));
    if (!pdf_.RenderPage(pageIndex_, renderWidth_, renderHeight_, pixels_)) return;
    ApplyInvert();
}

void AppWindow::ApplyInvert() {
    invertSettings_.enabled = invert_;
    InvertBgra(pixels_, invertSettings_);
}

void AppWindow::Paint(HDC hdc) {
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const COLORREF bg = invert_ ? RGB(26,26,26) : RGB(235,235,235);
    HBRUSH brush = CreateSolidBrush(bg); FillRect(hdc, &rc, brush); DeleteObject(brush);

    if (!pdf_.IsOpen() || pixels_.empty()) {
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(60,60,60));
        const wchar_t* text = L"打开 PDF  ·  Ctrl+O    |    I：反色    |    ← →：翻页";
        DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    const int x = (rc.right - renderWidth_) / 2;
    const int y = (rc.bottom - renderHeight_) / 2;
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = renderWidth_;
    bmi.bmiHeader.biHeight = -renderHeight_;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, x, y, renderWidth_, renderHeight_, 0, 0, renderWidth_, renderHeight_,
                  pixels_.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);

    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, invert_ ? RGB(220,220,220) : RGB(50,50,50));
    wchar_t status[128];
    wsprintfW(status, L"第 %d / %d 页    |    %s    |    Ctrl+O 打开    I 反色",
              pageIndex_ + 1, pdf_.PageCount(), invert_ ? L"反色" : L"正常");
    RECT footer = rc; footer.top = rc.bottom - 34;
    DrawTextW(hdc, status, -1, &footer, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
