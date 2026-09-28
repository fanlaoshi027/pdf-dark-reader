#include "AppWindow.h"
#include <algorithm>
#include <cmath>
#include <windowsx.h>
#include <commdlg.h>

namespace {
constexpr wchar_t kClassName[] = L"PDFDarkReaderWindow";
constexpr wchar_t kTitle[] = L"PDF Dark Reader";
constexpr int kToolbarHeight = 58;
constexpr int kButtonHeight = 38;
constexpr int kMargin = 12;
constexpr int kGap = 4;

enum : int {
    ID_OPEN = 1001, ID_SAVE = 1011,
    ID_ZOOM_OUT = 1004, ID_ZOOM_IN = 1005, ID_FIT_WIDTH = 1009,
    ID_PAN = 1401,
    ID_TOOL_PEN = 1301, ID_TOOL_RULER = 1402, ID_TOOL_LASSO = 1304,
    ID_TOOL_ERASER = 1303, ID_TOOL_LINE = 1302,
    ID_PEN_BLACK = 1501, ID_PEN_RED = 1502, ID_PEN_BLUE = 1503,
    ID_WIDTH_THIN = 1511, ID_WIDTH_MEDIUM = 1512, ID_WIDTH_THICK = 1513,
    ID_DASH = 1521, ID_ONE_STROKE = 1522
};

void InvertBgra(std::vector<std::uint8_t>& pixels, const InvertSettings& s) {
    if (!s.enabled) return;
    const float amount = std::clamp(s.strength, 0.0f, 1.0f);
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        const float b = pixels[i] / 255.0f, g = pixels[i + 1] / 255.0f, r = pixels[i + 2] / 255.0f;
        const float ir = r + (1.0f - 2.0f * r) * amount;
        const float ig = g + (1.0f - 2.0f * g) * amount;
        const float ib = b + (1.0f - 2.0f * b) * amount;
        const float orr = -0.574f * ir + 1.430f * ig + 0.144f * ib;
        const float org =  0.426f * ir + 0.430f * ig + 0.144f * ib;
        const float orb =  0.426f * ir + 1.430f * ig - 0.856f * ib;
        pixels[i] = static_cast<std::uint8_t>(std::lround(std::clamp(orb, 0.0f, 1.0f) * 255.0f));
        pixels[i + 1] = static_cast<std::uint8_t>(std::lround(std::clamp(org, 0.0f, 1.0f) * 255.0f));
        pixels[i + 2] = static_cast<std::uint8_t>(std::lround(std::clamp(orr, 0.0f, 1.0f) * 255.0f));
    }
}
}

bool AppWindow::Create(HINSTANCE instance) {
    instance_ = instance;
    invertSettings_.strength = 0.90f;
    invertSettings_.backgroundR = invertSettings_.backgroundG = invertSettings_.backgroundB = 26;
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc); wc.hInstance = instance; wc.lpfnWndProc = &AppWindow::WindowProc;
    wc.lpszClassName = kClassName; wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
    hwnd_ = CreateWindowExW(0, kClassName, kTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 850, nullptr, nullptr, instance, this);
    if (!hwnd_) return false;
    layers_.Create(hwnd_); layers_.SetTool(MosuanTool::Pen); CreateToolbar();
    ShowWindow(hwnd_, SW_SHOW); UpdateWindow(hwnd_); return true;
}

int AppWindow::Run() {
    MSG msg{}; while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK AppWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<AppWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam); self = static_cast<AppWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self)); self->hwnd_ = hwnd;
    }
    return self ? self->HandleMessage(message, wParam, lParam) : DefWindowProcW(hwnd, message, wParam, lParam);
}

void AppWindow::CreateToolbar() {
    toolbarFont_ = CreateFontW(23, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        FF_DONTCARE, L"Segoe UI Symbol");
    auto make = [&](const wchar_t* glyph, int id) {
        HWND w = CreateWindowW(L"BUTTON", glyph, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            0, 0, 40, kButtonHeight, hwnd_, reinterpret_cast<HMENU>(id), instance_, nullptr);
        SendMessageW(w, WM_SETFONT, reinterpret_cast<WPARAM>(toolbarFont_), TRUE); return w;
    };
    openButton_ = make(L"↥", ID_OPEN); saveButton_ = make(L"▣", ID_SAVE);
    zoomOutButton_ = make(L"−", ID_ZOOM_OUT);
    zoomLabel_ = CreateWindowW(L"STATIC", L"100%", WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 54, kButtonHeight, hwnd_, nullptr, instance_, nullptr);
    zoomInButton_ = make(L"+", ID_ZOOM_IN); fitWidthButton_ = make(L"↔", ID_FIT_WIDTH); panButton_ = make(L"✋", ID_PAN);
    penButton_ = make(L"✎", ID_TOOL_PEN); rulerButton_ = make(L"▱", ID_TOOL_RULER); lassoButton_ = make(L"⌁", ID_TOOL_LASSO);
    eraserButton_ = make(L"⌫", ID_TOOL_ERASER); lineButton_ = make(L"╱", ID_TOOL_LINE);
    colorBlackButton_ = make(L"●", ID_PEN_BLACK); colorRedButton_ = make(L"●", ID_PEN_RED); colorBlueButton_ = make(L"●", ID_PEN_BLUE);
    thinButton_ = make(L"━", ID_WIDTH_THIN); mediumButton_ = make(L"━━", ID_WIDTH_MEDIUM); thickButton_ = make(L"━━━", ID_WIDTH_THICK);
    dashButton_ = make(L"┄", ID_DASH); oneStrokeButton_ = make(L"∞", ID_ONE_STROKE);
    SendMessageW(zoomLabel_, WM_SETFONT, reinterpret_cast<WPARAM>(toolbarFont_), TRUE); LayoutToolbar(1200);
}

void AppWindow::LayoutToolbar(int width) {
    int x = kMargin; const int y = 10;
    auto place = [&](HWND w, int cw) { if (w) MoveWindow(w, x, y, cw, kButtonHeight, TRUE); x += cw + kGap; };
    const int bw = 38;
    place(openButton_, bw); place(saveButton_, bw); place(zoomOutButton_, bw); place(zoomLabel_, 52); place(zoomInButton_, bw);
    place(fitWidthButton_, bw); place(panButton_, bw); x += 10;
    place(penButton_, bw); place(rulerButton_, bw); place(lassoButton_, bw); place(eraserButton_, bw); place(lineButton_, bw); x += 10;
    place(colorBlackButton_, bw); place(colorRedButton_, bw); place(colorBlueButton_, bw); x += 6;
    place(thinButton_, bw); place(mediumButton_, bw); place(thickButton_, bw); x += 6;
    place(dashButton_, bw); place(oneStrokeButton_, bw);
    if (pageLabel_) MoveWindow(pageLabel_, x + 8, y, (std::max)(80, width - x - 16), kButtonHeight, TRUE);
}

void AppWindow::UpdateToolbarText() {
    wchar_t zoomText[32]; if (fitWidth_) lstrcpyW(zoomText, L"↔");
    else wsprintfW(zoomText, L"%d%%", static_cast<int>(std::lround(zoom_ * 100.0)));
    SetWindowTextW(zoomLabel_, zoomText); SendMessageW(zoomLabel_, WM_SETFONT, reinterpret_cast<WPARAM>(toolbarFont_), TRUE);
}

void AppWindow::DrawToolbarButton(const DRAWITEMSTRUCT* dis) {
    HDC dc = dis->hDC; RECT r = dis->rcItem; const int id = static_cast<int>(dis->CtlID);
    const bool selected =
        (id == ID_TOOL_PEN && activeTool_ == MosuanTool::Pen) || (id == ID_TOOL_LINE && activeTool_ == MosuanTool::Line) ||
        (id == ID_TOOL_ERASER && activeTool_ == MosuanTool::Eraser) || (id == ID_TOOL_LASSO && activeTool_ == MosuanTool::Lasso) ||
        (id == ID_PEN_BLACK && penColorIndex_ == 0) || (id == ID_PEN_RED && penColorIndex_ == 1) || (id == ID_PEN_BLUE && penColorIndex_ == 2) ||
        (id == ID_WIDTH_THIN && penWidthIndex_ == 0) || (id == ID_WIDTH_MEDIUM && penWidthIndex_ == 1) || (id == ID_WIDTH_THICK && penWidthIndex_ == 2) ||
        (id == ID_DASH && dashMode_) || (id == ID_ONE_STROKE && oneStrokeMode_);
    HBRUSH bg = CreateSolidBrush(selected ? RGB(48,111,205) : RGB(35,40,48)); FillRect(dc, &r, bg); DeleteObject(bg);
    if (selected) {
        HPEN border = CreatePen(PS_SOLID, 1, RGB(85,155,245)); HGDIOBJ oldPen = SelectObject(dc, border);
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));
        RoundRect(dc, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1, 8, 8);
        SelectObject(dc, oldBrush); SelectObject(dc, oldPen); DeleteObject(border);
    }
    COLORREF text = RGB(235,238,242);
    if (id == ID_PEN_BLACK) text = RGB(25,25,25); if (id == ID_PEN_RED) text = RGB(235,55,65); if (id == ID_PEN_BLUE) text = RGB(55,105,245);
    SetBkMode(dc, TRANSPARENT); SetTextColor(dc, text); wchar_t glyph[8]{}; GetWindowTextW(dis->hwndItem, glyph, 8);
    HFONT oldFont = static_cast<HFONT>(SelectObject(dc, toolbarFont_)); DrawTextW(dc, glyph, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE); SelectObject(dc, oldFont);
}

void AppWindow::SetMosuanTool(MosuanTool tool) { activeTool_ = tool; layers_.SetTool(tool); InvalidateRect(hwnd_, nullptr, FALSE); }

LRESULT AppWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_OPEN: OpenPdf(); return 0;
        case ID_SAVE: MessageBoxW(hwnd_, L"保存功能下一阶段接入。", L"墨算", MB_OK); return 0;
        case ID_ZOOM_OUT: fitWidth_ = false; ChangeZoom(0.8); return 0;
        case ID_ZOOM_IN: fitWidth_ = false; ChangeZoom(1.25); return 0;
        case ID_FIT_WIDTH: FitWidth(); return 0;
        case ID_PAN: return 0;
        case ID_TOOL_PEN: SetMosuanTool(MosuanTool::Pen); return 0;
        case ID_TOOL_RULER: return 0;
        case ID_TOOL_LASSO: SetMosuanTool(MosuanTool::Lasso); return 0;
        case ID_TOOL_ERASER: SetMosuanTool(MosuanTool::Eraser); return 0;
        case ID_TOOL_LINE: SetMosuanTool(MosuanTool::Line); return 0;
        case ID_PEN_BLACK: penColorIndex_ = 0; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_PEN_RED: penColorIndex_ = 1; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_PEN_BLUE: penColorIndex_ = 2; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_WIDTH_THIN: penWidthIndex_ = 0; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_WIDTH_MEDIUM: penWidthIndex_ = 1; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_WIDTH_THICK: penWidthIndex_ = 2; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_DASH: dashMode_ = !dashMode_; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        case ID_ONE_STROKE: oneStrokeMode_ = !oneStrokeMode_; InvalidateRect(hwnd_, nullptr, FALSE); return 0;
        }
        break;
    case WM_DRAWITEM: DrawToolbarButton(reinterpret_cast<DRAWITEMSTRUCT*>(lParam)); return TRUE;
    case WM_KEYDOWN:
        if (wParam == 'O' && (GetKeyState(VK_CONTROL) & 0x8000)) { OpenPdf(); return 0; }
        if (wParam == 'I' && pdf_.IsOpen()) { SetInvert(!invert_); return 0; }
        if (wParam == VK_LEFT) { GoPage(-1); return 0; } if (wParam == VK_RIGHT) { GoPage(1); return 0; }
        if (wParam == VK_UP) { ScrollBy(-80); return 0; } if (wParam == VK_DOWN) { ScrollBy(80); return 0; }
        if (wParam == VK_PRIOR) { ScrollBy(-400); return 0; } if (wParam == VK_NEXT) { ScrollBy(400); return 0; }
        break;
    case WM_MOUSEWHEEL: ScrollBy(-(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA) * 90); return 0;
    case WM_VSCROLL:
        switch (LOWORD(wParam)) {
        case SB_LINEUP: ScrollBy(-60); break; case SB_LINEDOWN: ScrollBy(60); break; case SB_PAGEUP: ScrollBy(-400); break; case SB_PAGEDOWN: ScrollBy(400); break;
        case SB_THUMBPOSITION: case SB_THUMBTRACK: { SCROLLINFO si{}; si.cbSize = sizeof(si); si.fMask = SIF_TRACKPOS; GetScrollInfo(hwnd_, SB_VERT, &si); scrollY_ = si.nTrackPos; UpdateLayerGeometry(); InvalidateRect(hwnd_, nullptr, FALSE); break; }
        default: break; }
        return 0;
    case WM_SIZE: LayoutToolbar(static_cast<int>(LOWORD(lParam))); if (pdf_.IsOpen()) RenderCurrentPage(); return 0;
    case WM_PAINT: { PAINTSTRUCT ps{}; HDC hdc = BeginPaint(hwnd_, &ps); Paint(hdc); EndPaint(hwnd_, &ps); return 0; }
    case WM_DESTROY: if (toolbarFont_) { DeleteObject(toolbarFont_); toolbarFont_ = nullptr; } PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd_, message, wParam, lParam);
}

void AppWindow::OpenPdf() {
    OPENFILENAMEW ofn{}; wchar_t file[MAX_PATH]{}; ofn.lStructSize = sizeof(ofn); ofn.hwndOwner = hwnd_; ofn.lpstrFile = file; ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"PDF files (*.pdf)\0*.pdf\0All files (*.*)\0*.*\0"; ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameW(&ofn)) {
        if (pdf_.Open(file)) { pageIndex_ = 0; invert_ = false; zoom_ = 1.0; fitWidth_ = false; scrollY_ = 0; layers_.ClearInk(); SetMosuanTool(MosuanTool::Pen); FitPage(); }
        else MessageBoxW(hwnd_, L"无法打开这个 PDF 文件。", L"PDF Dark Reader", MB_ICONERROR);
    }
}
void AppWindow::RenderCurrentPage() {
    if (!pdf_.IsOpen()) return; RECT rc{}; GetClientRect(hwnd_, &rc);
    const int availableW = (std::max)(200, static_cast<int>(rc.right) - 40); const int availableH = (std::max)(200, static_cast<int>(rc.bottom) - kToolbarHeight - 20);
    float pageW = 1, pageH = 1; if (!pdf_.PageSize(pageIndex_, pageW, pageH)) return;
    const double fitScale = std::min(static_cast<double>(availableW) / pageW, static_cast<double>(availableH) / pageH);
    const double scale = fitWidth_ ? static_cast<double>(availableW) / pageW : fitScale * zoom_;
    renderWidth_ = (std::max)(1, static_cast<int>(pageW * scale)); renderHeight_ = (std::max)(1, static_cast<int>(pageH * scale));
    if (!pdf_.RenderPage(pageIndex_, renderWidth_, renderHeight_, pixels_)) return; ApplyInvert();
    const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight); const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20);
    scrollY_ = std::clamp(scrollY_, 0, maxScroll); UpdateScrollBar(); UpdateToolbarText(); UpdateLayerGeometry(); InvalidateRect(hwnd_, nullptr, FALSE);
}
void AppWindow::ApplyInvert() { invertSettings_.enabled = invert_; InvertBgra(pixels_, invertSettings_); }
void AppWindow::ChangeZoom(double factor) {
    if (!pdf_.IsOpen()) return; fitWidth_ = false; RECT rc{}; GetClientRect(hwnd_, &rc); const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight);
    const int oldHeight = (std::max)(1, renderHeight_); const double centerRatio = std::clamp((scrollY_ + viewportH * 0.5) / static_cast<double>(oldHeight), 0.0, 1.0);
    zoom_ = std::clamp(zoom_ * factor, 0.5, 4.0); RenderCurrentPage(); const int newMax = (std::max)(0, renderHeight_ - viewportH + 20);
    scrollY_ = std::clamp(static_cast<int>(centerRatio * renderHeight_ - viewportH * 0.5), 0, newMax); UpdateScrollBar(); UpdateLayerGeometry(); InvalidateRect(hwnd_, nullptr, FALSE);
}
void AppWindow::FitPage() { fitWidth_ = false; zoom_ = 1.0; scrollY_ = 0; RenderCurrentPage(); }
void AppWindow::FitWidth() { if (!pdf_.IsOpen()) return; fitWidth_ = true; scrollY_ = 0; RenderCurrentPage(); }
void AppWindow::SetInvert(bool enabled) { invert_ = enabled; if (pdf_.IsOpen()) RenderCurrentPage(); }
void AppWindow::GoPage(int delta) {
    if (!pdf_.IsOpen()) return; const int next = pageIndex_ + delta; if (next < 0 || next >= pdf_.PageCount()) return;
    pageIndex_ = next; scrollY_ = 0; layers_.ClearInk(); RenderCurrentPage();
}
void AppWindow::ScrollBy(int delta) {
    if (!pdf_.IsOpen()) return; RECT rc{}; GetClientRect(hwnd_, &rc); const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight);
    const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20); if (maxScroll > 0) {
        const int old = scrollY_; scrollY_ = std::clamp(scrollY_ + delta, 0, maxScroll); if (old != scrollY_) { UpdateScrollBar(); UpdateLayerGeometry(); InvalidateRect(hwnd_, nullptr, FALSE); } return;
    }
    if (delta > 0) GoPage(-1); else if (delta < 0) GoPage(1);
}
void AppWindow::UpdateScrollBar() {
    RECT rc{}; GetClientRect(hwnd_, &rc); const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight); const int maxScroll = (std::max)(0, renderHeight_ - viewportH + 20);
    SCROLLINFO si{}; si.cbSize = sizeof(si); si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; si.nMin = 0; si.nMax = maxScroll; si.nPage = static_cast<UINT>(viewportH); si.nPos = std::clamp(scrollY_, 0, maxScroll); SetScrollInfo(hwnd_, SB_VERT, &si, TRUE);
}
void AppWindow::UpdateLayerGeometry() {
    RECT rc{}; GetClientRect(hwnd_, &rc); RECT viewport{0, kToolbarHeight, rc.right, rc.bottom}; layers_.Resize(viewport); float pageW = 1, pageH = 1; if (!pdf_.PageSize(pageIndex_, pageW, pageH)) return;
    const int availableW = (std::max)(1, static_cast<int>(rc.right) - 20); const int x = (std::max)(10, (availableW - renderWidth_) / 2); const int y = kToolbarHeight + 10 - scrollY_;
    const double scale = pageW > 0.0 ? static_cast<double>(renderWidth_) / pageW : 1.0; PdfViewTransform t{}; t.scale = scale; t.originX = x; t.originY = y - kToolbarHeight;
    t.pageWidth = static_cast<int>(pageW); t.pageHeight = static_cast<int>(pageH); layers_.SetTransform(t);
}
void AppWindow::ChooseBackground() {}
void AppWindow::ShowLayerMenu() {}

void AppWindow::Paint(HDC hdc) {
    RECT rc{}; GetClientRect(hwnd_, &rc);
    const COLORREF bg = invert_ ? RGB(invertSettings_.backgroundR, invertSettings_.backgroundG, invertSettings_.backgroundB) : RGB(235,235,235);
    HBRUSH brush = CreateSolidBrush(bg); FillRect(hdc, &rc, brush); DeleteObject(brush);

    HBRUSH toolbarBrush = CreateSolidBrush(RGB(28,33,41)); HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, toolbarBrush));
    HPEN toolbarPen = CreatePen(PS_SOLID, 1, RGB(65,73,84)); HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, toolbarPen));
    RECT toolbarRect{8,4,rc.right-8,kToolbarHeight-4}; RoundRect(hdc, toolbarRect.left, toolbarRect.top, toolbarRect.right, toolbarRect.bottom, 22,22);
    SelectObject(hdc, oldPen); SelectObject(hdc, oldBrush); DeleteObject(toolbarPen); DeleteObject(toolbarBrush);

    // Group dividers: PDF | 墨算 | 笔/线属性
    HPEN divider = CreatePen(PS_SOLID, 1, RGB(83,91,103)); oldPen = static_cast<HPEN>(SelectObject(hdc, divider));
    const int firstDivider = kMargin + 2*38 + 6*0 + 3*38 + 52 + 6*4;
    MoveToEx(hdc, firstDivider + 1, 15, nullptr); LineTo(hdc, firstDivider + 1, kToolbarHeight - 15);
    const int secondDivider = firstDivider + 10 + 5*38 + 4*4 + 10;
    MoveToEx(hdc, secondDivider + 1, 15, nullptr); LineTo(hdc, secondDivider + 1, kToolbarHeight - 15);
    SelectObject(hdc, oldPen); DeleteObject(divider);

    if (!pdf_.IsOpen() || pixels_.empty()) {
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(70,70,70)); RECT body = rc; body.top = kToolbarHeight;
        DrawTextW(hdc, L"打开 PDF", -1, &body, DT_CENTER | DT_VCENTER | DT_SINGLELINE); return;
    }
    const int viewportTop = kToolbarHeight, viewportBottom = static_cast<int>(rc.bottom); const int availableW = static_cast<int>(rc.right) - 20;
    const int x = (std::max)(10, (availableW - renderWidth_) / 2); const int y = viewportTop + 10 - scrollY_;
    if (y < viewportBottom && y + renderHeight_ > viewportTop) {
        BITMAPINFO bmi{}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth = renderWidth_; bmi.bmiHeader.biHeight = -renderHeight_;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(hdc, x, y, renderWidth_, renderHeight_, 0,0,renderWidth_,renderHeight_,pixels_.data(),&bmi,DIB_RGB_COLORS,SRCCOPY);
    }
}
