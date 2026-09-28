#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>

namespace {
constexpr wchar_t kMainClass[] = L"PDFDarkReaderWindow";
constexpr wchar_t kToolbarClass[] = L"MosuanIconToolbar";
constexpr int kToolbarH = 54;
constexpr int kButtonH = 40;
constexpr int kGap = 5;
constexpr int kSep = 10;
constexpr int kSaveId = 2101;
constexpr int kPanId = 2102;
constexpr int kRulerId = 2103;
constexpr int kDashId = 2104;
constexpr int kSmoothId = 2105;
constexpr int kColorBlack = 2106;
constexpr int kColorRed = 2107;
constexpr int kColorBlue = 2108;

struct Item { HWND h{}; int id{}; std::wstring icon; bool selected{}; bool toggle{}; };
struct State {
    HWND main{};
    HWND bar{};
    std::vector<Item> items;
    bool dash{};
    bool smooth{};
};
State g;

void PaintButton(LPDRAWITEMSTRUCT d, const Item& item) {
    HDC dc=d->hDC; RECT r=d->rcItem;
    HBRUSH bg=CreateSolidBrush(item.selected ? RGB(42,111,214) : RGB(28,38,51));
    FillRect(dc,&r,bg); DeleteObject(bg);
    HPEN border=CreatePen(PS_SOLID,1,item.selected ? RGB(76,150,255) : RGB(62,74,91));
    HGDIOBJ old=SelectObject(dc,border); RoundRect(dc,r.left,r.top,r.right,r.bottom,10,10); SelectObject(dc,old); DeleteObject(border);
    SetBkMode(dc,TRANSPARENT); SetTextColor(dc,RGB(242,246,252));
    HFONT font=CreateFontW(24,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI Symbol");
    old=SelectObject(dc,font); DrawTextW(dc,item.icon.c_str(),-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE); SelectObject(dc,old); DeleteObject(font);
    if(item.toggle && item.selected){
        HBRUSH dot=CreateSolidBrush(RGB(255,255,255)); RECT q{r.right-10,r.top+5,r.right-5,r.top+10}; FillRect(dc,&q,dot); DeleteObject(dot);
    }
}

void Repaint(){ if(g.bar) InvalidateRect(g.bar,nullptr,FALSE); }

void Add(int id,const wchar_t* icon,int w=42,bool selected=false,bool toggle=false){
    HWND h=CreateWindowExW(0,L"BUTTON",icon,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,0,0,w,kButtonH,g.bar,reinterpret_cast<HMENU>(id),GetModuleHandleW(nullptr),nullptr);
    g.items.push_back({h,id,icon,selected,toggle});
}
void Sep(){ Add(0,L"│",12,false,false); }

void Layout(){
    if(!g.main || !g.bar) return;
    RECT rc{}; GetClientRect(g.main,&rc);
    int x=8;
    for(auto& it:g.items){
        int w=42; if(it.id==0) w=12;
        if(it.id==kSaveId || it.id==kPanId || it.id==kRulerId || it.id==kDashId || it.id==kSmoothId) w=42;
        if(it.id==kColorBlack || it.id==kColorRed || it.id==kColorBlue) w=38;
        MoveWindow(it.h,x,7,w,kButtonH,TRUE); x+=w+kGap;
    }
    MoveWindow(g.bar,0,0,rc.right,kToolbarH,TRUE);
}

Item* Find(int id){ for(auto& i:g.items) if(i.id==id) return &i; return nullptr; }
void Forward(int id){ if(g.main && id) SendMessageW(g.main,WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),0); }

LRESULT CALLBACK ToolbarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_DRAWITEM:{ auto* d=reinterpret_cast<LPDRAWITEMSTRUCT>(lp); if(!d) break; Item* item=Find(static_cast<int>(d->CtlID)); if(item){PaintButton(d,*item);return TRUE;} break; }
    case WM_COMMAND:{ int id=LOWORD(wp); Item* item=Find(id); if(!item) break;
        if(id==kDashId || id==kSmoothId){ item->selected=!item->selected; if(id==kDashId)g.dash=item->selected; else g.smooth=item->selected; Repaint(); return 0; }
        if(id==kColorBlack || id==kColorRed || id==kColorBlue){
            for(auto& i:g.items) if(i.id==kColorBlack||i.id==kColorRed||i.id==kColorBlue) i.selected=(i.id==id);
            Repaint(); return 0;
        }
        if(id==kSaveId){ /* Save command is intentionally UI-only in this pass. */ return 0; }
        if(id==kPanId || id==kRulerId){ item->selected=!item->selected; Repaint(); return 0; }
        if(id==1001 || id==1004 || id==1005 || id==1009 || id==1301 || id==1303 || id==1304 || id==1302){
            for(auto& i:g.items) if(i.id==1301||i.id==1302||i.id==1303||i.id==1304||i.id==kPanId||i.id==kRulerId) i.selected=(i.id==id);
            Repaint(); Forward(id); return 0;
        }
        Forward(id); return 0;
    }
    case WM_SIZE: Layout(); return 0;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT:{ PAINTSTRUCT ps{}; HDC dc=BeginPaint(hwnd,&ps); RECT r{};GetClientRect(hwnd,&r);HBRUSH b=CreateSolidBrush(RGB(18,27,38));FillRect(dc,&r,b);DeleteObject(b);EndPaint(hwnd,&ps);return 0; }
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

void HideLegacyToolbar(){
    EnumChildWindows(g.main,[](HWND h,LPARAM)->BOOL{
        wchar_t cls[32]{};GetClassNameW(h,cls,32);
        if(lstrcmpW(cls,L"BUTTON")==0 || lstrcmpW(cls,L"STATIC")==0) ShowWindow(h,SW_HIDE);
        return TRUE;
    },0);
}

void CreateUi(){
    if(g.bar || !g.main) return;
    WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=GetModuleHandleW(nullptr);wc.lpfnWndProc=ToolbarProc;wc.lpszClassName=kToolbarClass;wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);wc.hbrBackground=nullptr;RegisterClassExW(&wc);
    HideLegacyToolbar();
    g.bar=CreateWindowExW(0,kToolbarClass,L"",WS_CHILD|WS_VISIBLE,0,0,1000,kToolbarH,g.main,nullptr,GetModuleHandleW(nullptr),nullptr);
    Add(1001,L"⌂"); Add(kSaveId,L"▣"); Sep(); Add(1004,L"−"); Add(1005,L"+"); Add(1009,L"↔"); Add(kPanId,L"✋"); Sep();
    Add(1301,L"✎",42,true); Add(kRulerId,L"▤"); Add(1304,L"⌁"); Add(1303,L"◇"); Add(1302,L"╱"); Sep();
    Add(kColorBlack,L"●",38,true); Add(kColorRed,L"●",38); Add(kColorBlue,L"●",38); Sep();
    Add(kDashId,L"━━",42,false,true); Add(kSmoothId,L"〰",42,false,true);
    Layout();
}

DWORD WINAPI ThreadProc(void*){
    for(int i=0;i<100;++i){ g.main=FindWindowW(kMainClass,nullptr); if(g.main){CreateUi();break;} Sleep(100); }
    while(g.main && IsWindow(g.main)){ Layout(); Sleep(250); }
    return 0;
}
struct Start{Start(){CreateThread(nullptr,0,ThreadProc,nullptr,0,nullptr);}} g_start;
}
