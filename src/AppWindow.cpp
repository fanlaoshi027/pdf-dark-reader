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
    ID_OPEN = 1001, ID_PREV = 1002, ID_NEXT = 1003,
    ID_ZOOM_OUT = 1004, ID_ZOOM_IN = 1005, ID_FIT = 1006,
    ID_INVERT = 1007, ID_COLOR = 1008,
    ID_COLOR_BLACK = 1101, ID_COLOR_CHARCOAL = 1102,
    ID_COLOR_BLUE = 1103, ID_COLOR_CUSTOM = 1104
};

// This deliberately mirrors the website player filter:
//     filter: invert(90%) hue-rotate(180deg)
// The important point is that the filter is applied to the final rendered
// page pixels, not to PDF drawing commands. That keeps anti-aliased text,
// scanned pages, vector pages and images in one consistent pipeline.
void InvertBgra(std::vector<std::uint8_t>& pixels, const InvertSettings& s) {
    if (!s.enabled) return;

    const float amount = std::clamp(s.strength, 0.0f, 1.0f);

    // CSS invert(amount), followed by hue-rotate(180deg), represented as
    // one RGB matrix. The hue-rotate matrix is applied after interpolation
    // toward the inverted value, exactly like the browser filter pipeline.
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        const float b = pixels[i] / 255.0f;
        const float g = pixels[i + 1] / 255.0f;
        const float r = pixels[i + 2] / 255.0f;

        // invert(amount): amount=0 leaves the channel unchanged;
        // amount=1 produces 1-channel.
        const float ir = r + (1.0f - 2.0f * r) * amount;
        const float ig = g + (1.0f - 2.0f * g) * amount;
        const float ib = b + (1.0f - 2.0f * b) * amount;

        // hue-rotate(180deg), using the CSS Filter Effects RGB matrix.
        const float orr = -0.574f * ir + 1.430f * ig + 0.144f * ib;
        const float org =  0.426f * ir + 0.430f * ig + 0.144f * ib;
        const float orb =  0.426f * ir + 1.430f * ig - 0.856f * ib;

        pixels[i]     = static_cast<std::uint8_t>(std::lround(std::clamp(orb, 0.0f, 1.0f) * 255.0f));
        pixels[i + 1] = static_cast<std::uint8_t>(std::lround(std::clamp(org, 0.0f, 1.0f) * 255.0f));
        pixels[i + 2] = static_cast<std::uint8_t>(std::lround(std::clamp(orr, 0.0f, 1.0f) * 255.0f));
    }
}

const wchar_t* BackgroundName(const InvertSettings& s) {
    if (s.backgroundR == 26 && s.backgroundG == 26 && s.backgroundB == 26) return L"炭黑";
    if (s.backgroundR == 16 && s.backgroundG == 24 && s.backgroundB == 39) return L"深蓝";
    if (s.backgroundR == 12 && s.backgroundG == 12 && s.backgroundB == 12) return L"深黑";
    return L"自定义";
}
}

bool AppWindow::Create(HINSTANCE instance) {
    instance_ = instance;
    // 90% is intentionally the same as the website's invert(90%).
    invertSettings_.strength = 0.90f;
    invertSettings_.backgroundR = 26;
    invertSettings_.backgroundG = 26;
    invertSettings_.backgroundB = 26;

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
    openButton_ = CreateWindowW(L"BUTTON", L"打开", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,70,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_OPEN),instance_,nullptr);
    prevButton_ = CreateWindowW(L"BUTTON", L"上一页", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,70,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_PREV),instance_,nullptr);
    nextButton_ = CreateWindowW(L"BUTTON", L"下一页", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,70,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_NEXT),instance_,nullptr);
    zoomOutButton_ = CreateWindowW(L"BUTTON", L"−", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,34,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_ZOOM_OUT),instance_,nullptr);
    zoomLabel_ = CreateWindowW(L"STATIC", L"100%", WS_CHILD | WS_VISIBLE | SS_CENTER, 0,0,54,kButtonHeight,hwnd_,nullptr,instance_,nullptr);
    zoomInButton_ = CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,34,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_ZOOM_IN),instance_,nullptr);
    fitButton_ = CreateWindowW(L"BUTTON", L"适合", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,54,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_FIT),instance_,nullptr);
    invertButton_ = CreateWindowW(L"BUTTON", L"反色", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,60,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_INVERT),instance_,nullptr);
    colorButton_ = CreateWindowW(L"BUTTON", L"炭黑", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,0,60,kButtonHeight,hwnd_,reinterpret_cast<HMENU>(ID_COLOR),instance_,nullptr);
    pageLabel_ = CreateWindowW(L"STATIC", L"未打开 PDF", WS_CHILD | WS_VISIBLE | SS_CENTER, 0,0,120,kButtonHeight,hwnd_,nullptr,instance_,nullptr);
    LayoutToolbar(1200);
}

void AppWindow::LayoutToolbar(int width) {
    int x = kMargin; const int y = 8; const int gap = 4;
    auto place = [&](HWND w, int cw) { if (w) MoveWindow(w,x,y,cw,kButtonHeight,TRUE); x += cw + gap; };
    place(openButton_,62); place(prevButton_,62); place(nextButton_,62);
    place(zoomOutButton_,32); place(zoomLabel_,52); place(zoomInButton_,32);
    place(fitButton_,52); place(invertButton_,56); place(colorButton_,60);
    if (pageLabel_) MoveWindow(pageLabel_,x+4,y,(std::max)(90,width-x-14),kButtonHeight,TRUE);
}

void AppWindow::UpdateToolbarText() {
    wchar_t zoomText[32];
    wsprintfW(zoomText,L"%d%%",static_cast<int>(std::lround(zoom_*100.0)));
    SetWindowTextW(zoomLabel_,zoomText);
    SetWindowTextW(colorButton_,BackgroundName(invertSettings_));
    if (!pdf_.IsOpen()) SetWindowTextW(pageLabel_,L"未打开 PDF");
    else { wchar_t pageText[64]; wsprintfW(pageText,L"第 %d / %d 页",pageIndex_+1,pdf_.PageCount()); SetWindowTextW(pageLabel_,pageText); }
    SetWindowTextW(invertButton_,invert_ ? L"正常" : L"反色");
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
        case ID_FIT: FitPage(); return 0;
        case ID_INVERT: SetInvert(!invert_); return 0;
        case ID_COLOR: ChooseBackground(); return 0;
        case ID_COLOR_BLACK: invertSettings_.backgroundR=invertSettings_.backgroundG=invertSettings_.backgroundB=12; UpdateToolbarText(); if(invert_) RenderCurrentPage(); return 0;
        case ID_COLOR_CHARCOAL: invertSettings_.backgroundR=invertSettings_.backgroundG=invertSettings_.backgroundB=26; UpdateToolbarText(); if(invert_) RenderCurrentPage(); return 0;
        case ID_COLOR_BLUE: invertSettings_.backgroundR=16; invertSettings_.backgroundG=24; invertSettings_.backgroundB=39; UpdateToolbarText(); if(invert_) RenderCurrentPage(); return 0;
        case ID_COLOR_CUSTOM: {
            CHOOSECOLORW cc{}; static COLORREF custom[16]{};
            cc.lStructSize=sizeof(cc); cc.hwndOwner=hwnd_;
            cc.rgbResult=RGB(invertSettings_.backgroundR,invertSettings_.backgroundG,invertSettings_.backgroundB);
            cc.lpCustColors=custom; cc.Flags=CC_FULLOPEN|CC_RGBINIT;
            if(ChooseColorW(&cc)) {
                invertSettings_.backgroundR=GetRValue(cc.rgbResult); invertSettings_.backgroundG=GetGValue(cc.rgbResult); invertSettings_.backgroundB=GetBValue(cc.rgbResult);
                UpdateToolbarText(); if(invert_) RenderCurrentPage();
            }
            return 0;
        }
        default: break;
        }
        break;
    case WM_KEYDOWN:
        if(wParam=='O' && (GetKeyState(VK_CONTROL)&0x8000)){OpenPdf();return 0;}
        if(wParam=='I' && pdf_.IsOpen()){SetInvert(!invert_);return 0;}
        if(wParam==VK_LEFT){GoPage(-1);return 0;}
        if(wParam==VK_RIGHT){GoPage(1);return 0;}
        if(wParam==VK_UP){ScrollBy(-80);return 0;}
        if(wParam==VK_DOWN){ScrollBy(80);return 0;}
        if(wParam==VK_PRIOR){ScrollBy(-400);return 0;}
        if(wParam==VK_NEXT){ScrollBy(400);return 0;}
        break;
    case WM_MOUSEWHEEL:
        ScrollBy(-(GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA)*90); return 0;
    case WM_VSCROLL:
        switch(LOWORD(wParam)) {
        case SB_LINEUP: ScrollBy(-60); break;
        case SB_LINEDOWN: ScrollBy(60); break;
        case SB_PAGEUP: ScrollBy(-400); break;
        case SB_PAGEDOWN: ScrollBy(400); break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK: {
            SCROLLINFO si{}; si.cbSize=sizeof(si); si.fMask=SIF_TRACKPOS;
            GetScrollInfo(hwnd_,SB_VERT,&si); scrollY_=si.nTrackPos;
            InvalidateRect(hwnd_,nullptr,FALSE); break;
        }
        default: break;
        }
        return 0;
    case WM_SIZE:
        LayoutToolbar(static_cast<int>(LOWORD(lParam)));
        if(pdf_.IsOpen()) RenderCurrentPage();
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps{}; HDC hdc=BeginPaint(hwnd_,&ps); Paint(hdc); EndPaint(hwnd_,&ps); return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd_,message,wParam,lParam);
}

void AppWindow::OpenPdf() {
    OPENFILENAMEW ofn{}; wchar_t file[MAX_PATH]{};
    ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=hwnd_; ofn.lpstrFile=file; ofn.nMaxFile=MAX_PATH;
    ofn.lpstrFilter=L"PDF files (*.pdf)\0*.pdf\0All files (*.*)\0*.*\0";
    ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
    if(GetOpenFileNameW(&ofn)) {
        if(pdf_.Open(file)){pageIndex_=0;invert_=false;zoom_=1.0;scrollY_=0;FitPage();}
        else MessageBoxW(hwnd_,L"无法打开这个 PDF 文件。",L"PDF Dark Reader",MB_ICONERROR);
    }
}

void AppWindow::RenderCurrentPage() {
    if(!pdf_.IsOpen()) return;
    RECT rc{}; GetClientRect(hwnd_,&rc);
    const int availableW=(std::max)(200,static_cast<int>(rc.right)-40);
    const int availableH=(std::max)(200,static_cast<int>(rc.bottom)-kToolbarHeight-20);
    float pageW=1,pageH=1; if(!pdf_.PageSize(pageIndex_,pageW,pageH)) return;
    const double fitScale=std::min(static_cast<double>(availableW)/pageW,static_cast<double>(availableH)/pageH);
    const double scale=fitScale*zoom_;
    renderWidth_=(std::max)(1,static_cast<int>(pageW*scale));
    renderHeight_=(std::max)(1,static_cast<int>(pageH*scale));
    if(!pdf_.RenderPage(pageIndex_,renderWidth_,renderHeight_,pixels_)) return;
    ApplyInvert();
    const int viewportH=(std::max)(1,static_cast<int>(rc.bottom)-kToolbarHeight);
    const int maxScroll=(std::max)(0,renderHeight_-viewportH+20);
    scrollY_=std::clamp(scrollY_,0,maxScroll);
    UpdateScrollBar(); UpdateToolbarText(); InvalidateRect(hwnd_,nullptr,FALSE);
}

void AppWindow::ApplyInvert(){
    invertSettings_.enabled=invert_;
    InvertBgra(pixels_,invertSettings_);
}

void AppWindow::ChangeZoom(double factor) {
    if(!pdf_.IsOpen()) return;
    RECT rc{}; GetClientRect(hwnd_,&rc);
    const int viewportH=(std::max)(1,static_cast<int>(rc.bottom)-kToolbarHeight);
    const int oldHeight=(std::max)(1,renderHeight_);
    const double centerRatio=std::clamp((scrollY_+viewportH*0.5)/static_cast<double>(oldHeight),0.0,1.0);
    zoom_=std::clamp(zoom_*factor,0.5,4.0); RenderCurrentPage();
    const int newMax=(std::max)(0,renderHeight_-viewportH+20);
    scrollY_=std::clamp(static_cast<int>(centerRatio*renderHeight_-viewportH*0.5),0,newMax);
    UpdateScrollBar(); InvalidateRect(hwnd_,nullptr,FALSE);
}

void AppWindow::FitPage(){zoom_=1.0;scrollY_=0;RenderCurrentPage();}
void AppWindow::SetInvert(bool enabled){invert_=enabled;if(pdf_.IsOpen())RenderCurrentPage();else UpdateToolbarText();}

void AppWindow::GoPage(int delta){
    if(!pdf_.IsOpen()) return; const int next=pageIndex_+delta;
    if(next<0||next>=pdf_.PageCount()) return;
    pageIndex_=next;scrollY_=0;RenderCurrentPage();
}

void AppWindow::ScrollBy(int delta){
    if(!pdf_.IsOpen()) return;
    RECT rc{};GetClientRect(hwnd_,&rc);
    const int viewportH=(std::max)(1,static_cast<int>(rc.bottom)-kToolbarHeight);
    const int maxScroll=(std::max)(0,renderHeight_-viewportH+20);
    if(maxScroll>0){
        const int old=scrollY_;scrollY_=std::clamp(scrollY_+delta,0,maxScroll);
        if(old!=scrollY_){UpdateScrollBar();InvalidateRect(hwnd_,nullptr,FALSE);}return;
    }
    if(delta>0)GoPage(-1);else if(delta<0)GoPage(1);
}

void AppWindow::UpdateScrollBar(){
    RECT rc{};GetClientRect(hwnd_,&rc);
    const int viewportH=(std::max)(1,static_cast<int>(rc.bottom)-kToolbarHeight);
    const int maxScroll=(std::max)(0,renderHeight_-viewportH+20);
    SCROLLINFO si{};si.cbSize=sizeof(si);si.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
    si.nMin=0;si.nMax=maxScroll;si.nPage=static_cast<UINT>(viewportH);si.nPos=std::clamp(scrollY_,0,maxScroll);
    SetScrollInfo(hwnd_,SB_VERT,&si,TRUE);
}

void AppWindow::ChooseBackground(){
    POINT p{};GetCursorPos(&p);HMENU menu=CreatePopupMenu();
    AppendMenuW(menu,MF_STRING,ID_COLOR_BLACK,L"深黑");
    AppendMenuW(menu,MF_STRING,ID_COLOR_CHARCOAL,L"炭黑");
    AppendMenuW(menu,MF_STRING,ID_COLOR_BLUE,L"深蓝");
    AppendMenuW(menu,MF_SEPARATOR,0,nullptr);
    AppendMenuW(menu,MF_STRING,ID_COLOR_CUSTOM,L"自定义颜色…");
    TrackPopupMenu(menu,TPM_RIGHTBUTTON|TPM_TOPALIGN,p.x,p.y,0,hwnd_,nullptr);DestroyMenu(menu);
}

void AppWindow::Paint(HDC hdc){
    RECT rc{};GetClientRect(hwnd_,&rc);
    const COLORREF bg=invert_?RGB(invertSettings_.backgroundR,invertSettings_.backgroundG,invertSettings_.backgroundB):RGB(235,235,235);
    HBRUSH brush=CreateSolidBrush(bg);FillRect(hdc,&rc,brush);DeleteObject(brush);
    HBRUSH toolbarBrush=CreateSolidBrush(invert_?RGB(32,32,32):RGB(248,248,248));
    RECT toolbarRect{0,0,rc.right,kToolbarHeight};FillRect(hdc,&toolbarRect,toolbarBrush);DeleteObject(toolbarBrush);
    if(!pdf_.IsOpen()||pixels_.empty()){
        SetBkMode(hdc,TRANSPARENT);SetTextColor(hdc,invert_?RGB(225,225,225):RGB(70,70,70));
        RECT body=rc;body.top=kToolbarHeight;
        DrawTextW(hdc,L"点击“打开”选择 PDF",-1,&body,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;
    }
    const int viewportTop=kToolbarHeight;const int viewportBottom=static_cast<int>(rc.bottom);
    const int availableW=static_cast<int>(rc.right)-20;
    const int x=(std::max)(10,(availableW-renderWidth_)/2);
    const int y=viewportTop+10-scrollY_;
    if(y<viewportBottom&&y+renderHeight_>viewportTop){
        BITMAPINFO bmi{};bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth=renderWidth_;bmi.bmiHeader.biHeight=-renderHeight_;
        bmi.bmiHeader.biPlanes=1;bmi.bmiHeader.biBitCount=32;bmi.bmiHeader.biCompression=BI_RGB;
        StretchDIBits(hdc,x,y,renderWidth_,renderHeight_,0,0,renderWidth_,renderHeight_,pixels_.data(),&bmi,DIB_RGB_COLORS,SRCCOPY);
    }
}
