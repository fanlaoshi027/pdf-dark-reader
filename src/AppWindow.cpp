#include "AppWindow.h"
#include <algorithm>
#include <cmath>
#include <windowsx.h>
#include <commdlg.h>

namespace {
constexpr wchar_t kClassName[] = L"PDFDarkReaderWindow";
constexpr wchar_t kTitle[] = L"PDF Dark Reader";
constexpr int kToolbarHeight = 46;
constexpr int kButtonHeight = 30;
constexpr int kMargin = 7;

enum : int {
    ID_OPEN = 1001,
    ID_PREV = 1002,
    ID_NEXT = 1003,
    ID_ZOOM_OUT = 1004,
    ID_ZOOM_IN = 1005,
    ID_FIT = 1006,
    ID_INVERT = 1007
};

void InvertBgra(std::vector<std::uint8_t>& pixels, const InvertSettings& s) {
    if (!s.enabled) return;
    const float strength = std::clamp(s.strength, 0.0f, 1.0f);
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        const float b0 = static_cast<float>(pixels[i]);
        const float g0 = static_cast<float>(pixels[i + 1]);
        const float r0 = static_cast<float>(pixels[i + 2]);

        // Invert only neutral pixels (black/white/gray). Preserve hue for
        // colored annotations, pens and diagrams.
        const float maxC = (std::max)({ r0, g0, b0 });
        const float minC = (std::min)({ r0, g0, b0 });
        const bool neutral = (maxC - minC) <= 18.0f;
        if (!neutral) continue;

        const float b = 255.0f - b0;
        const float g = 255.0f - g0;
        const float r = 255.0f - r0;
        pixels[i]     = static_cast<std::uint8_t>(std::clamp(s.backgroundB + (b - s.backgroundB) * strength, 0.0f, 255.0f));
        pixels[i + 1] = static_cast<std::uint8_t>(std::clamp(s.backgroundG + (g - s.backgroundG) * strength, 0.0f, 255.0f));
        pixels[i + 2] = static_cast<std::uint8_t>(std::clamp(s.backgroundR + (r - s.backgroundR) * strength, 0.0f, 255.0f));
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

    hwnd_ = CreateWindowExW(0, kClassName, kTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 850, nullptr, nullptr, instance, this);
    if (!hwnd_) return false;

    CreateToolbar();
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
    return self ? self->HandleMessage(message, wParam, lParam) : DefWindowProcW(hwnd, message, wParam, lParam);
}

void AppWindow::CreateToolbar() {
    openButton_ = CreateWindowW(L"BUTTON", L"打开", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 70, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_OPEN), instance_, nullptr);
    prevButton_ = CreateWindowW(L"BUTTON", L"上一页", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 70, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_PREV), instance_, nullptr);
    nextButton_ = CreateWindowW(L"BUTTON", L"下一页", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 70, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_NEXT), instance_, nullptr);
    zoomOutButton_ = CreateWindowW(L"BUTTON", L"−", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 34, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_ZOOM_OUT), instance_, nullptr);
    zoomLabel_ = CreateWindowW(L"STATIC", L"100%", WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 54, kButtonHeight, hwnd_, nullptr, instance_, nullptr);
    zoomInButton_ = CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 34, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_ZOOM_IN), instance_, nullptr);
    fitButton_ = CreateWindowW(L"BUTTON", L"适合", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 54, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_FIT), instance_, nullptr);
    invertButton_ = CreateWindowW(L"BUTTON", L"反色", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 60, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(ID_INVERT), instance_, nullptr);
    pageLabel_ = CreateWindowW(L"STATIC", L"未打开 PDF", WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 120, kButtonHeight, hwnd_, nullptr, instance_, nullptr);
    LayoutToolbar(1200);
}

void AppWindow::LayoutToolbar(int width) {
    int x = kMargin;
    const int y = 8;
    const int gap = 5;
    auto place = [&](HWND w, int cw) {
        if (w) MoveWindow(w, x, y, cw, kButtonHeight, TRUE);
        x += cw + gap;
    };
    place(openButton_, 70);
    place(prevButton_, 70);
    place(nextButton_, 70);
    place(zoomOutButton_, 34);
    place(zoomLabel_, 54);
    place(zoomInButton_, 34);
    place(fitButton_, 54);
    place(invertButton_, 60);
    if (pageLabel_) MoveWindow(pageLabel_, x + 8, y, (std::max)(100, width - x - 18), kButtonHeight, TRUE);
}

void AppWindow::UpdateToolbarText() {
    wchar_t zoomText[32];
    wsprintfW(zoomText, L"%d%%", static_cast<int>(std::lround(zoom_ * 100.0)));
    SetWindowTextW(zoomLabel_, zoomText);

    if (!pdf_.IsOpen()) {
        SetWindowTextW(pageLabel_, L"未打开 PDF");
    } else {
        wchar_t pageText[64];
        wsprintfW(pageText, L"第 %d / %d 页", pageIndex_ + 1, pdf_.PageCount());
        SetWindowTextW(pageLabel_, pageText);
    }
    SetWindowTextW(invertButton_, invert_ ? L"正常" : L"反色");
}

LRESULT AppWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_OPEN: OpenPdf(); return 0;
        case ID_PREV: GoPage(-1); return 0;
        case ID_NEXT: GoPage(1); return 0;
        case ID_ZOOM_OUT: ChangeZoom(0.8); return 0;
        case ID_ZOOM_IN: ChangeZoom(1.25); return 0;
        case ID_FIT: zoom_ = 1.0; scrollY_ = 0; RenderCurrentPage(); return 0;
        case ID_INVERT: SetInvert(!invert_); return 0;
        default: break;
        }
        break;

    case WM_KEYDOWN:
        if (wParam == 'O' && (GetKeyState(VK_CONTROL) & 0x8000)) { OpenPdf(); return 0; }
        if (wParam == 'I' && pdf_.IsOpen()) { SetInvert(!invert_); return 0; }
        if (wParam == VK_LEFT) { GoPage(-1); return 0; }
        if (wParam == VK_RIGHT) { GoPage(1); return 0; }
        if (wParam == VK_UP) { ScrollBy(-80); return 0; }
        if (wParam == VK_DOWN) { ScrollBy(80); return 0; }
        if (wParam == VK_PRIOR) { ScrollBy(-400); return 0; }
        if (wParam == VK_NEXT) { ScrollBy(400); return 0; }
        break;

    case WM_MOUSEWHEEL: {
        const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        ScrollBy(-(delta / 120) * 90);
        return 0;
    }

    case WM_VSCROLL:
        switch (LOWORD(wParam)) {
        case SB_LINEUP: ScrollBy(-60); break;
        case SB_LINEDOWN: ScrollBy(60); break;
        case SB_PAGEUP: ScrollBy(-400); break;
        case SB_PAGEDOWN: ScrollBy(400); break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK: {
            SCROLLINFO si{}; si.cbSize = sizeof(si); si.fMask = SIF_TRACKPOS;
            GetScrollInfo(hwnd_, SB_VERT, &si);
            scrollY_ = si.nTrackPos;
            InvalidateRect(hwnd_, nullptr, FALSE);
            break;
        }
        default: break;
        }
        return 0;

    case WM_SIZE:
        LayoutToolbar(LOWORD(lParam));
        if (pdf_.IsOpen()) RenderCurrentPage();
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd_, &ps);
        Paint(hdc);
        EndPaint(hwnd_, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
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
            zoom_ = 1.0;
            scrollY_ = 0;
            RenderCurrentPage();
        } else {
            MessageBoxW(hwnd_, L"无法打开这个 PDF 文件。", L"PDF Dark Reader", MB_ICONERROR);
        }
    }
}

void AppWindow::RenderCurrentPage() {
    if (!pdf_.IsOpen()) return;
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const int availableW = (std::max)(200, static_cast<int>(rc.right - 40));
    const int availableH = (std::max)(200, static_cast<int>(rc.bottom - kToolbarHeight - 20));
    float pageW = 1, pageH = 1;
    if (!pdf_.PageSize(pageIndex_, pageW, pageH)) return;

    const double fitScale = std::min(static_cast<double>(availableW) / pageW,
                                     static_cast<double>(availableH) / pageH);
    const double scale = fitScale * zoom_;
    renderWidth_ = (std::max)(1, static_cast<int>(pageW * scale));
    renderHeight_ = (std::max)(1, static_cast<int>(pageH * scale));

    if (!pdf_.RenderPage(pageIndex_, renderWidth_, renderHeight_, pixels_)) return;
    ApplyInvert();

    const int viewportH = (std::max)(1, rc.bottom - kToolbarHeight);
    const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20);
    scrollY_ = std::clamp(scrollY_, 0, maxScroll);
    UpdateScrollBar();
    UpdateToolbarText();
    InvalidateRect(hwnd_, nullptr, FALSE);
}

void AppWindow::ApplyInvert() {
    invertSettings_.enabled = invert_;
    InvertBgra(pixels_, invertSettings_);
}

void AppWindow::ChangeZoom(double factor) {
    if (!pdf_.IsOpen()) return;
    zoom_ = std::clamp(zoom_ * factor, 0.5, 4.0);
    scrollY_ = 0;
    RenderCurrentPage();
}

void AppWindow::SetInvert(bool enabled) {
    invert_ = enabled;
    if (pdf_.IsOpen()) RenderCurrentPage();
    else UpdateToolbarText();
}

void AppWindow::GoPage(int delta) {
    if (!pdf_.IsOpen()) return;
    const int next = pageIndex_ + delta;
    if (next < 0 || next >= pdf_.PageCount()) return;
    pageIndex_ = next;
    scrollY_ = 0;
    RenderCurrentPage();
}

void AppWindow::ScrollBy(int delta) {
    if (!pdf_.IsOpen()) return;
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const int viewportH = (std::max)(1, rc.bottom - kToolbarHeight);
    const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20);

    if (maxScroll > 0) {
        const int old = scrollY_;
        scrollY_ = std::clamp(scrollY_ + delta, 0, maxScroll);
        if (old != scrollY_) {
            UpdateScrollBar();
            InvalidateRect(hwnd_, nullptr, FALSE);
        }
        return;
    }

    // At fit-to-page size, mouse-wheel/vertical scrolling becomes page navigation.
    if (delta > 0) GoPage(-1);
    else if (delta < 0) GoPage(1);
}

void AppWindow::UpdateScrollBar() {
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const int viewportH = (std::max)(1, rc.bottom - kToolbarHeight);
    const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20);

    SCROLLINFO si{};
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = (std::max)(0, maxScroll);
    si.nPage = static_cast<UINT>(viewportH);
    si.nPos = std::clamp(scrollY_, 0, maxScroll);
    SetScrollInfo(hwnd_, SB_VERT, &si, TRUE);
}

void AppWindow::Paint(HDC hdc) {
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const COLORREF bg = invert_ ? RGB(26,26,26) : RGB(235,235,235);
    HBRUSH brush = CreateSolidBrush(bg);
    FillRect(hdc, &rc, brush);
    DeleteObject(brush);

    HBRUSH toolbarBrush = CreateSolidBrush(invert_ ? RGB(32,32,32) : RGB(248,248,248));
    RECT toolbarRect{ 0, 0, rc.right, kToolbarHeight };
    FillRect(hdc, &toolbarRect, toolbarBrush);
    DeleteObject(toolbarBrush);

    if (!pdf_.IsOpen() || pixels_.empty()) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, invert_ ? RGB(225,225,225) : RGB(70,70,70));
        RECT body = rc; body.top = kToolbarHeight;
        const wchar_t* text = L"点击“打开”选择 PDF";
        DrawTextW(hdc, text, -1, &body, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    const int viewportTop = kToolbarHeight;
    const int viewportBottom = rc.bottom;
    const int availableW = rc.right - 20;
    const int x = (std::max)(10, (availableW - renderWidth_) / 2);
    const int y = viewportTop + 10 - scrollY_;

    if (y < viewportBottom && y + renderHeight_ > viewportTop) {
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = renderWidth_;
        bmi.bmiHeader.biHeight = -renderHeight_;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(hdc, x, y, renderWidth_, renderHeight_, 0, 0, renderWidth_, renderHeight_,
                      pixels_.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
    }
}
