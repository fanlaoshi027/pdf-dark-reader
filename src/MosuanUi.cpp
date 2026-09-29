#include <windows.h>
#include <windowsx.h>
#include "MosuanUiState.h"
#include "MosuanUiPrimitives.h"
#include "MosuanUiTopBar.h"
#include "MosuanUiRightRail.h"
#include "MosuanUiPanels.h"

namespace MosuanUI {
static RECT clientRect(){RECT r{};GetClientRect(state().ui,&r);return r;}
static bool hit(POINT p,RECT r){return PtInRect(&r,p)!=FALSE;}
static void invalidate(){if(state().ui)InvalidateRect(state().ui,nullptr,FALSE);}
static void send(int id){if(state().main)SendMessageW(state().main,WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),0);}
static void handleTopClick(POINT p){auto&s=state();if(p.y>70)return;if(p.x>=18){int index=(p.x-18)/46;const int pdfIds[]={ID_OPEN,ID_SAVE,ID_ZOOM_OUT,ID_ZOOM_IN,ID_FIT_WIDTH,ID_PAN};if(index>=0&&index<6){send(pdfIds[index]);return;}const int tools[]={ID_TOOL_PEN,ID_TOOL_RULER,ID_TOOL_LASSO,ID_TOOL_ERASER,ID_TOOL_LINE};if(index>=7&&index<=11){s.selectedTool=tools[index-7];send(s.selectedTool);invalidate();return;}}
int x=18+12*46+12;const int colors[]={ID_PEN_BLACK,ID_PEN_RED,ID_PEN_BLUE};for(int i=0;i<3;i++){RECT b{x+i*42,14,x+i*42+38,52};if(hit(p,b)){s.color=colors[i];send(s.color);invalidate();return;}}x+=3*42+4;const int widths[]={ID_WIDTH_THIN,ID_WIDTH_MEDIUM,ID_WIDTH_THICK};for(int i=0;i<3;i++){RECT b{x+i*42,14,x+i*42+38,52};if(hit(p,b)){s.width=widths[i];send(s.width);invalidate();return;}}x+=3*42+4;if(hit(p,{x,14,x+38,52})){s.dashed=!s.dashed;send(ID_DASH);invalidate();return;}x+=44;if(hit(p,{x,14,x+38,52})){s.oneStroke=!s.oneStroke;send(ID_ONE_STROKE);invalidate();return;}}
static void hideLegacy(HWND main){EnumChildWindows(main,[](HWND h,LPARAM)->BOOL{wchar_t cls[32]{};GetClassNameW(h,cls,32);if(lstrcmpW(cls,L"BUTTON")==0||lstrcmpW(cls,L"STATIC")==0)ShowWindow(h,SW_HIDE);return TRUE;},0);}
static LRESULT CALLBACK wndProc(HWND w,UINT msg,WPARAM wp,LPARAM lp){switch(msg){case WM_LBUTTONDOWN:{POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};RECT rc=clientRect();if(p.y<=70){handleTopClick(p);return 0;}if(handleRightRailClick(p,rc)){invalidate();return 0;}if(handlePanelClick(p,rc)){invalidate();return 0;}return 0;}case WM_NCHITTEST:{POINT p{};GetCursorPos(&p);ScreenToClient(w,&p);RECT rc=clientRect();int x=rc.right-kRightRail-4;if(p.y<=70||p.x>=x-8||state().layersOpen||state().settingsOpen)return HTCLIENT;return HTTRANSPARENT;}case WM_PAINT:{PAINTSTRUCT ps{};HDC dc=BeginPaint(w,&ps);RECT rc{};GetClientRect(w,&rc);drawTopBar(dc,rc);drawRightRail(dc,rc);drawLayerPanel(dc,rc);drawSettingsPanel(dc,rc);EndPaint(w,&ps);return 0;}case WM_ERASEBKGND:return 1;case WM_SIZE:invalidate();return 0;}return DefWindowProcW(w,msg,wp,lp);}
static DWORD WINAPI uiThread(void*){auto&s=state();for(int i=0;i<100&&!s.main;i++){s.main=FindWindowW(kMainClass,nullptr);if(!s.main)Sleep(100);}if(!s.main)return 0;hideLegacy(s.main);WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=GetModuleHandleW(nullptr);wc.lpfnWndProc=wndProc;wc.lpszClassName=kUiClass;wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);RegisterClassExW(&wc);s.ui=CreateWindowExW(0,kUiClass,L"",WS_CHILD|WS_VISIBLE,0,0,1000,1000,s.main,nullptr,wc.hInstance,nullptr);while(IsWindow(s.main)){RECT r{};GetClientRect(s.main,&r);SetWindowPos(s.ui,HWND_TOP,0,0,r.right,r.bottom,SWP_NOACTIVATE|SWP_SHOWWINDOW);Sleep(200);}return 0;}
struct Bootstrap{Bootstrap(){CreateThread(nullptr,0,uiThread,nullptr,0,nullptr);}};static Bootstrap bootstrap;
}
