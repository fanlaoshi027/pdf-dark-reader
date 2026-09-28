#include <windows.h>
#include <array>
#include <algorithm>

namespace {
constexpr wchar_t kMainClass[] = L"PDFDarkReaderWindow";
constexpr wchar_t kPopupClass[] = L"PDFDarkReaderFavoriteBar";
constexpr int kStarId = 1699;
constexpr int kSlotBase = 1700;
constexpr int kSlots = 6;
constexpr int kPenId = 1301;
constexpr int kLineId = 1302;

struct State {
    HWND main = nullptr;
    HWND star = nullptr;
    HWND bar = nullptr;
    std::array<HWND,kSlots> slots{};
    std::array<int,kSlots> tools{};
    std::array<bool,kSlots> valid{};
};
State g;

void Position() {
    if(!g.main) return;
    RECT rc{}; GetClientRect(g.main,&rc);
    POINT p{(std::min)(885L,(std::max)(0L,rc.right-52L)),7};
    ClientToScreen(g.main,&p);
    if(g.star) SetWindowPos(g.star,HWND_TOP,p.x,p.y,46,30,SWP_NOACTIVATE|SWP_SHOWWINDOW);
    POINT r{(std::max)(0L,rc.right-64L),84};
    ClientToScreen(g.main,&r);
    if(g.bar) SetWindowPos(g.bar,HWND_TOP,r.x,r.y,60,314,SWP_NOACTIVATE|SWP_SHOWWINDOW);
}

void UpdateSlots() {
    for(int i=0;i<kSlots;++i) if(g.slots[i]) {
        SetWindowTextW(g.slots[i], g.valid[i] ? (g.tools[i]==kLineId ? L"／" : L"✎") : L"＋");
    }
}

void SaveCurrent() {
    if(!g.main) return;
    HWND pen=FindWindowExW(g.main,nullptr,L"BUTTON",L"钢笔");
    HWND line=FindWindowExW(g.main,nullptr,L"BUTTON",L"直线");
    int tool = kPenId;
    if(line && (SendMessageW(line,BM_GETSTATE,0,0)&BST_PUSHED)) tool=kLineId;
    else if(pen && (SendMessageW(pen,BM_GETSTATE,0,0)&BST_PUSHED)) tool=kPenId;
    int slot=-1; for(int i=0;i<kSlots;++i) if(!g.valid[i]) {slot=i;break;}
    if(slot<0) slot=kSlots-1;
    g.valid[slot]=true; g.tools[slot]=tool; UpdateSlots();
}

void Activate(int i) {
    if(!g.main || i<0 || i>=kSlots || !g.valid[i]) return;
    SendMessageW(g.main,WM_COMMAND,MAKEWPARAM(g.tools[i],BN_CLICKED),0);
}

LRESULT CALLBACK PopupProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_COMMAND) {
        const int id=LOWORD(wp);
        if(id==kStarId){SaveCurrent();return 0;}
        if(id>=kSlotBase && id<kSlotBase+kSlots){Activate(id-kSlotBase);return 0;}
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

void CreateUi() {
    if(g.star || !g.main) return;
    HINSTANCE hi=GetModuleHandleW(nullptr);
    WNDCLASSEXW wc{}; wc.cbSize=sizeof(wc); wc.hInstance=hi; wc.lpfnWndProc=PopupProc; wc.lpszClassName=kPopupClass;
    wc.hCursor=LoadCursorW(nullptr,IDC_HAND); wc.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
    RegisterClassExW(&wc);
    g.star=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,kPopupClass,L"★",WS_POPUP|WS_VISIBLE,0,0,46,30,g.main,nullptr,hi,nullptr);
    g.bar=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,kPopupClass,L"",WS_POPUP,0,0,60,314,g.main,nullptr,hi,nullptr);
    for(int i=0;i<kSlots;++i) g.slots[i]=CreateWindowW(L"BUTTON",L"＋",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,6,5+i*51,48,46,g.bar,reinterpret_cast<HMENU>(kSlotBase+i),hi,nullptr);
    UpdateSlots(); Position(); ShowWindow(g.bar,SW_SHOWNOACTIVATE);
}

DWORD WINAPI ThreadProc(void*) {
    for(int i=0;i<100;i++) {
        g.main=FindWindowW(kMainClass,nullptr);
        if(g.main){CreateUi();break;}
        Sleep(100);
    }
    while(g.main && IsWindow(g.main)) { Position(); Sleep(250); }
    return 0;
}

struct Start { Start(){CreateThread(nullptr,0,ThreadProc,nullptr,0,nullptr);} } g_start;
}
