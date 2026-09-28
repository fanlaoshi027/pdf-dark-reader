#include <cmath>
#include <windows.h>
#include <windowsx.h>

namespace {
constexpr wchar_t kMainClass[] = L"PDFDarkReaderWindow";
constexpr wchar_t kToolbarClass[] = L"PDFDarkReaderIconToolbar";
constexpr int H = 46;
constexpr int ID_OPEN=1001, ID_ZOOM_OUT=1004, ID_ZOOM_IN=1005, ID_FIT_WIDTH=1009;
constexpr int ID_TOOL_PEN=1301, ID_TOOL_LINE=1302, ID_TOOL_ERASER=1303, ID_TOOL_LASSO=1304;

struct State { HWND main=nullptr, bar=nullptr; int color=0, width=1; bool dashed=false, oneStroke=false; int selectedTool=6; };
State g;
void Send(int id){ if(g.main) SendMessageW(g.main,WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),0); }
void Line(HDC h,int x1,int y1,int x2,int y2){MoveToEx(h,x1,y1,nullptr);LineTo(h,x2,y2);}
void Circle(HDC h,int x,int y,int r){Ellipse(h,x-r,y-r,x+r,y+r);}
void DrawIcon(HDC h,int id,int x,int y,bool active){
    COLORREF stroke=RGB(230,235,242), fill=active?RGB(60,120,205):RGB(32,38,48);
    if(id==11) fill=RGB(25,25,25); if(id==12) fill=RGB(220,45,55); if(id==13) fill=RGB(45,90,210);
    HPEN p=CreatePen(PS_SOLID,(id>=11&&id<=13)?1:2,stroke);
    HBRUSH b=CreateSolidBrush(fill); auto op=SelectObject(h,p), ob=SelectObject(h,b);
    switch(id){
    case 0: Rectangle(h,x+5,y+9,x+21,y+21);Line(h,x+5,y+9,x+11,y+9);Line(h,x+11,y+9,x+14,y+12);break;
    case 1: Rectangle(h,x+5,y+6,x+21,y+22);Rectangle(h,x+9,y+7,x+17,y+12);Rectangle(h,x+9,y+16,x+18,y+21);break;
    case 2: Line(h,x+7,y+14,x+21,y+14);break;
    case 3: Line(h,x+14,y+7,x+14,y+21);Line(h,x+7,y+14,x+21,y+14);break;
    case 4: Line(h,x+6,y+9,x+22,y+9);Line(h,x+6,y+19,x+22,y+19);Line(h,x+6,y+9,x+9,y+6);Line(h,x+6,y+9,x+9,y+12);Line(h,x+22,y+19,x+19,y+16);Line(h,x+22,y+19,x+19,y+22);break;
    case 5: Line(h,x+14,y+21,x+14,y+10);Line(h,x+14,y+10,x+11,y+7);Line(h,x+11,y+7,x+11,y+15);Line(h,x+11,y+15,x+8,y+11);Line(h,x+8,y+11,x+7,y+12);Line(h,x+7,y+12,x+10,y+18);Line(h,x+10,y+18,x+18,y+22);Line(h,x+18,y+22,x+21,y+17);break;
    case 6: Line(h,x+7,y+21,x+21,y+7);Line(h,x+7,y+21,x+11,y+20);Line(h,x+21,y+7,x+18,y+6);break;
    case 7: Rectangle(h,x+7,y+7,x+21,y+21);Line(h,x+10,y+10,x+10,y+14);Line(h,x+14,y+10,x+14,y+16);Line(h,x+18,y+10,x+18,y+14);break;
    case 8: Ellipse(h,x+6,y+7,x+22,y+21);Line(h,x+18,y+18,x+23,y+23);break;
    case 9: {POINT pts[4]={{x+7,y+18},{x+13,y+7},{x+22,y+12},{x+16,y+22}};Polygon(h,pts,4);}break;
    case 10: Line(h,x+7,y+21,x+21,y+7);break;
    case 11: Circle(h,x+14,y+14,6);break;
    case 12: Circle(h,x+14,y+14,6);break;
    case 13: Circle(h,x+14,y+14,6);break;
    case 14: {HPEN q=CreatePen(PS_SOLID,1,stroke);HGDIOBJ old=SelectObject(h,q);Line(h,x+6,y+14,x+22,y+14);SelectObject(h,old);DeleteObject(q);break;}
    case 15: Line(h,x+6,y+14,x+22,y+14);break;
    case 16: {HPEN q=CreatePen(PS_SOLID,4,stroke);HGDIOBJ old=SelectObject(h,q);Line(h,x+6,y+14,x+22,y+14);SelectObject(h,old);DeleteObject(q);break;}
    case 17: Line(h,x+6,y+14,x+10,y+14);Line(h,x+14,y+14,x+18,y+14);break;
    case 18: {Arc(h,x+6,y+7,x+22,y+21,0,0,0,0);Line(h,x+7,y+18,x+20,y+9);break;}
    }
    SelectObject(h,ob);SelectObject(h,op);DeleteObject(b);DeleteObject(p);
}
LRESULT CALLBACK Proc(HWND w,UINT m,WPARAM wp,LPARAM lp){
    switch(m){
    case WM_LBUTTONDOWN:{int x=GET_X_LPARAM(lp);int pos=x-8;if(pos<42){Send(ID_OPEN);return 0;}pos-=46;if(pos<42){return 0;}pos-=46;if(pos<42){Send(ID_ZOOM_OUT);return 0;}pos-=46;if(pos<42){Send(ID_ZOOM_IN);return 0;}pos-=46;if(pos<42){Send(ID_FIT_WIDTH);return 0;}pos-=56;if(pos<42){return 0;}pos-=46;if(pos<42){Send(ID_TOOL_PEN);g.selectedTool=6;InvalidateRect(w,nullptr,FALSE);return 0;}pos-=46;if(pos<42){return 0;}pos-=46;if(pos<42){Send(ID_TOOL_LASSO);g.selectedTool=8;InvalidateRect(w,nullptr,FALSE);return 0;}pos-=46;if(pos<42){Send(ID_TOOL_ERASER);g.selectedTool=9;InvalidateRect(w,nullptr,FALSE);return 0;}pos-=46;if(pos<42){Send(ID_TOOL_LINE);g.selectedTool=10;InvalidateRect(w,nullptr,FALSE);return 0;}return 0;}
    case WM_PAINT:{PAINTSTRUCT ps{};HDC h=BeginPaint(w,&ps);RECT r{};GetClientRect(w,&r);HBRUSH bg=CreateSolidBrush(RGB(25,31,40));FillRect(h,&r,bg);DeleteObject(bg);int x=8;for(int i=0;i<2;i++){DrawIcon(h,i,x,8,false);x+=46;}MoveToEx(h,x-7,5,nullptr);LineTo(h,x-7,41);x+=8;for(int id=2;id<=5;id++){DrawIcon(h,id,x,8,false);x+=46;}MoveToEx(h,x-7,5,nullptr);LineTo(h,x-7,41);x+=8;for(int id=6;id<=10;id++){DrawIcon(h,id,x,8,g.selectedTool==id);x+=46;}MoveToEx(h,x-7,5,nullptr);LineTo(h,x-7,41);x+=8;DrawIcon(h,11,x,8,g.color==0);x+=38;DrawIcon(h,12,x,8,g.color==1);x+=38;DrawIcon(h,13,x,8,g.color==2);x+=46;DrawIcon(h,14,x,8,g.width==0);x+=38;DrawIcon(h,15,x,8,g.width==1);x+=38;DrawIcon(h,16,x,8,g.width==2);x+=46;DrawIcon(h,17,x,8,g.dashed);x+=46;DrawIcon(h,18,x,8,g.oneStroke);EndPaint(w,&ps);return 0;}
    case WM_SIZE:InvalidateRect(w,nullptr,FALSE);return 0;}
    return DefWindowProcW(w,m,wp,lp);
}
void Position(){if(!g.main||!g.bar)return;RECT r{};GetClientRect(g.main,&r);SetWindowPos(g.bar,HWND_TOP,0,0,r.right,H,SWP_NOACTIVATE|SWP_SHOWWINDOW);}
DWORD WINAPI Thread(void*){for(int i=0;i<100;i++){g.main=FindWindowW(kMainClass,nullptr);if(g.main)break;Sleep(100);}if(!g.main)return 0;HINSTANCE hi=GetModuleHandleW(nullptr);WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=hi;wc.lpfnWndProc=Proc;wc.lpszClassName=kToolbarClass;wc.hCursor=LoadCursorW(nullptr,IDC_HAND);RegisterClassExW(&wc);g.bar=CreateWindowExW(0,kToolbarClass,L"",WS_CHILD|WS_VISIBLE,0,0,800,H,g.main,nullptr,hi,nullptr);Position();while(IsWindow(g.main)){Position();Sleep(250);}return 0;}
struct Start{Start(){CreateThread(nullptr,0,Thread,nullptr,0,nullptr);}} start;
}
