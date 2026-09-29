#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace {
constexpr wchar_t kMainClass[] = L"PDFDarkReaderWindow";
constexpr wchar_t kUiClass[] = L"MosuanUiOverlay";
constexpr int UI_RIGHT = 58;
constexpr int UI_SLOT = 46;
constexpr int UI_GAP = 7;
constexpr int UI_PANEL_W = 360;

constexpr int ID_OPEN=1001, ID_SAVE=1011, ID_ZOOM_OUT=1004, ID_ZOOM_IN=1005, ID_FIT_WIDTH=1009, ID_PAN=1401;
constexpr int ID_TOOL_PEN=1301, ID_TOOL_RULER=1402, ID_TOOL_LASSO=1304, ID_TOOL_ERASER=1303, ID_TOOL_LINE=1302;
constexpr int ID_PEN_BLACK=1501, ID_PEN_RED=1502, ID_PEN_BLUE=1503;
constexpr int ID_WIDTH_THIN=1511, ID_WIDTH_MEDIUM=1512, ID_WIDTH_THICK=1513, ID_DASH=1521, ID_ONE_STROKE=1522;
constexpr UINT WM_MOSUAN_NOTE_VIS = WM_APP + 31;
constexpr UINT WM_MOSUAN_BG_VIS = WM_APP + 32;

struct Slot { bool valid=false; int command=ID_TOOL_PEN; };
struct State {
    HWND main=nullptr; HWND ui=nullptr;
    bool layers=false, settings=false, background=true, notes=true, singlePage=true;
    int selectedTool=ID_TOOL_PEN, color=ID_PEN_BLACK, width=ID_WIDTH_MEDIUM;
    bool dashed=false, oneStroke=true;
    std::array<Slot,6> slots{};
};
State g;

COLORREF C(int r,int g2,int b){return RGB(r,g2,b);}
void FillRound(HDC h,const RECT& r,COLORREF fill,int radius=14){HBRUSH b=CreateSolidBrush(fill);HPEN p=CreatePen(PS_SOLID,1,fill);auto ob=SelectObject(h,b);auto op=SelectObject(h,p);RoundRect(h,r.left,r.top,r.right,r.bottom,radius,radius);SelectObject(h,op);SelectObject(h,ob);DeleteObject(p);DeleteObject(b);}
void StrokeRound(HDC h,const RECT& r,COLORREF stroke,int radius=14){HPEN p=CreatePen(PS_SOLID,1,stroke);auto op=SelectObject(h,p);auto ob=SelectObject(h,GetStockObject(HOLLOW_BRUSH));RoundRect(h,r.left,r.top,r.right,r.bottom,radius,radius);SelectObject(h,ob);SelectObject(h,op);DeleteObject(p);}
void Line(HDC h,int x1,int y1,int x2,int y2){MoveToEx(h,x1,y1,nullptr);LineTo(h,x2,y2);}
void Circle(HDC h,int x,int y,int r,COLORREF fill){HBRUSH b=CreateSolidBrush(fill);auto ob=SelectObject(h,b);Ellipse(h,x-r,y-r,x+r,y+r);SelectObject(h,ob);DeleteObject(b);}

void Icon(HDC h,int kind,RECT r,bool active=false){
    const int cx=(r.left+r.right)/2, cy=(r.top+r.bottom)/2; const COLORREF white=C(232,237,244); COLORREF line=active?C(245,248,252):white;
    HPEN p=CreatePen(PS_SOLID,2,line); auto op=SelectObject(h,p); auto ob=SelectObject(h,GetStockObject(HOLLOW_BRUSH));
    switch(kind){
    case 0: Rectangle(h,cx-9,cy-8,cx+9,cy+8); Line(h,cx-5,cy+4,cx+5,cy+4); break;
    case 1: Rectangle(h,cx-9,cy-8,cx+9,cy+8); Rectangle(h,cx-6,cy-6,cx+6,cy-2); Line(h,cx-5,cy+8,cx-5,cy+3); break;
    case 2: Line(h,cx+7,cy,cx-7,cy); Line(h,cx-7,cy,cx-2,cy-5); Line(h,cx-7,cy,cx-2,cy+5); break;
    case 3: Line(h,cx-7,cy,cx+7,cy); Line(h,cx+7,cy,cx+2,cy-5); Line(h,cx+7,cy,cx+2,cy+5); break;
    case 4: Line(h,cx-8,cy,cx+8,cy); Line(h,cx-8,cy-5,cx-8,cy+5); Line(h,cx+8,cy-5,cx+8,cy+5); break;
    case 5: Line(h,cx,cy+8,cx,cy-7); Line(h,cx,cy-7,cx-3,cy-4); Line(h,cx-3,cy-4,cx-3,cy+1); Line(h,cx-3,cy+1,cx-7,cy-2); Line(h,cx-7,cy-2,cx-9,cy); Line(h,cx-9,cy,cx-5,cy+7); Line(h,cx-5,cy+7,cx+5,cy+9); Line(h,cx+5,cy+9,cx+8,cy+3); break;
    case 6: Line(h,cx-8,cy+8,cx+6,cy-6); Line(h,cx+6,cy-6,cx+9,cy-3); Line(h,cx+9,cy-3,cx-5,cy+10); Line(h,cx-8,cy+8,cx-2,cy+9); break;
    case 7: Rectangle(h,cx-9,cy-9,cx+9,cy+9); Line(h,cx-5,cy-4,cx+5,cy-4); Line(h,cx-5,cy,cx+3,cy); Line(h,cx-5,cy+4,cx+5,cy+4); break;
    case 8: Ellipse(h,cx-8,cy-7,cx+7,cy+7); Line(h,cx+5,cy+5,cx+10,cy+10); break;
    case 9: {POINT q[4]={{cx-9,cy+4},{cx-2,cy-8},{cx+8,cy-3},{cx+1,cy+9}};Polygon(h,q,4);} break;
    case 10: Line(h,cx-8,cy+8,cx+8,cy-8); break;
    case 11: Ellipse(h,cx-6,cy-6,cx+6,cy+6); break;
    case 12: Ellipse(h,cx-6,cy-6,cx+6,cy+6); break;
    case 13: Ellipse(h,cx-6,cy-6,cx+6,cy+6); break;
    case 14: Line(h,cx-8,cy,cx+8,cy); break;
    case 15: {HPEN q=CreatePen(PS_SOLID,3,line);auto oq=SelectObject(h,q);Line(h,cx-8,cy,cx+8,cy);SelectObject(h,oq);DeleteObject(q);break;}
    case 16: {HPEN q=CreatePen(PS_SOLID,6,line);auto oq=SelectObject(h,q);Line(h,cx-8,cy,cx+8,cy);SelectObject(h,oq);DeleteObject(q);break;}
    case 17: Line(h,cx-8,cy,cx-3,cy); Line(h,cx,cy,cx+5,cy); break;
    case 18: Arc(h,cx-8,cy-8,cx+8,cy+8,0,0,0,0); Line(h,cx-6,cy+7,cx+7,cy-7); break;
    case 19: {POINT s[10]{};for(int i=0;i<10;i++){double a=-3.14159265/2+i*3.14159265/5;int rr=(i%2==0?10:4);s[i]={cx+(int)std::lround(std::cos(a)*rr),cy+(int)std::lround(std::sin(a)*rr)};}Polygon(h,s,10);}break;
    case 20: Rectangle(h,cx-9,cy-7,cx+9,cy-2); Rectangle(h,cx-9,cy,cx+9,cy+5); Rectangle(h,cx-9,cy+7,cx+9,cy+12); break;
    case 21: Ellipse(h,cx-7,cy-7,cx+7,cy+7); Circle(h,cx,cy,2,active?white:C(28,33,41)); break;
    case 22: Arc(h,cx-10,cy-6,cx+10,cy+6,0,0,0,0); Ellipse(h,cx-3,cy-3,cx+3,cy+3); break;
    case 23: Rectangle(h,cx-7,cy-1,cx+7,cy+8); Arc(h,cx-5,cy-9,cx+5,cy+3,0,0,0,0); break;
    case 24: Line(h,cx-7,cy,cx+7,cy);Line(h,cx,cy-7,cx,cy+7);break;
    case 25: Line(h,cx,cy+7,cx,cy-7);Line(h,cx,cy-7,cx-5,cy-2);Line(h,cx,cy-7,cx+5,cy-2);break;
    case 26: Line(h,cx,cy-7,cx,cy+7);Line(h,cx,cy+7,cx-5,cy+2);Line(h,cx,cy+7,cx+5,cy+2);break;
    case 27: Rectangle(h,cx-6,cy-5,cx+6,cy+8);Line(h,cx-8,cy-8,cx+8,cy-8);Line(h,cx-3,cy-10,cx+3,cy-10);break;
    }
    SelectObject(h,ob);SelectObject(h,op);DeleteObject(p);
}
void Text(HDC h,const wchar_t* s,RECT r,int size=15,COLORREF color=C(230,235,242),UINT flags=DT_LEFT|DT_VCENTER|DT_SINGLELINE){HFONT f=CreateFontW(-size,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Microsoft YaHei UI");auto of=SelectObject(h,f);SetBkMode(h,TRANSPARENT);SetTextColor(h,color);DrawTextW(h,s,-1,&r,flags);SelectObject(h,of);DeleteObject(f);}
void Send(int id){if(g.main)SendMessageW(g.main,WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),0);} void InvalidateUi(){if(g.ui)InvalidateRect(g.ui,nullptr,FALSE);}
void AddFavorite(){for(auto&s:g.slots)if(!s.valid){s.valid=true;s.command=g.selectedTool;InvalidateUi();return;}g.slots.back().valid=true;g.slots.back().command=g.selectedTool;InvalidateUi();}
void ActivateSlot(int i){if(i>=0&&i<(int)g.slots.size()&&g.slots[i].valid){g.selectedTool=g.slots[i].command;Send(g.slots[i].command);InvalidateUi();}}
RECT ClientRect(){RECT r{};GetClientRect(g.ui,&r);return r;} bool Hit(POINT p,RECT r){return PtInRect(&r,p)!=FALSE;}
RECT TopButton(int index){int x=18+index*46;return RECT{x,14,x+38,52};}

void DrawTop(HDC h,RECT rc){
    RECT bar{10,8,rc.right-10,64};FillRound(h,bar,C(24,30,39),28);StrokeRound(h,bar,C(63,73,87),28);
    int ids[]={ID_OPEN,ID_SAVE,ID_ZOOM_OUT,ID_ZOOM_IN,ID_FIT_WIDTH,ID_PAN};int icons[]={0,1,2,3,4,5};
    for(int n=0;n<6;n++){RECT b=TopButton(n);bool a=(ids[n]==ID_PAN);FillRound(h,b,a?C(55,116,210):C(31,38,48),10);Icon(h,icons[n],b,a);}
    int sepX=18+6*46-4;HPEN p=CreatePen(PS_SOLID,1,C(78,87,101));auto op=SelectObject(h,p);Line(h,sepX,17,sepX,55);SelectObject(h,op);DeleteObject(p);
    int toolStart=7;int tids[]={ID_TOOL_PEN,ID_TOOL_RULER,ID_TOOL_LASSO,ID_TOOL_ERASER,ID_TOOL_LINE};int ticons[]={6,7,8,9,10};
    for(int n=0;n<5;n++){RECT b=TopButton(toolStart+n);bool a=g.selectedTool==tids[n];FillRound(h,b,a?C(55,116,210):C(31,38,48),10);Icon(h,ticons[n],b,a);}
    sepX=18+(toolStart+5)*46-4;p=CreatePen(PS_SOLID,1,C(78,87,101));op=SelectObject(h,p);Line(h,sepX,17,sepX,55);SelectObject(h,op);DeleteObject(p);
    int x=18+(toolStart+5)*46+12;int cids[]={ID_PEN_BLACK,ID_PEN_RED,ID_PEN_BLUE};COLORREF cols[]={C(15,17,20),C(238,55,70),C(65,100,235)};
    for(int n=0;n<3;n++){RECT b{x,14,x+38,52};if(g.color==cids[n])FillRound(h,b,C(40,48,60),10);Circle(h,(b.left+b.right)/2,(b.top+b.bottom)/2,8,cols[n]);x+=42;}
    x+=4;int wids[]={ID_WIDTH_THIN,ID_WIDTH_MEDIUM,ID_WIDTH_THICK};int wicons[]={14,15,16};for(int n=0;n<3;n++){RECT b{x,14,x+38,52};if(g.width==wids[n])FillRound(h,b,C(40,48,60),10);Icon(h,wicons[n],b,g.width==wids[n]);x+=42;}
    x+=4;RECT dash{x,14,x+38,52};if(g.dashed)FillRound(h,dash,C(55,116,210),10);Icon(h,17,dash,g.dashed);x+=44;RECT one{x,14,x+38,52};if(g.oneStroke)FillRound(h,one,C(55,116,210),10);Icon(h,18,one,g.oneStroke);
}
void DrawFavoriteBar(HDC h,RECT rc){
    int x=rc.right-UI_RIGHT-4,y=86;RECT bg{x-5,y-8,x+UI_RIGHT+2,y+6+6*UI_SLOT+5*UI_GAP};FillRound(h,bg,C(25,31,41),20);StrokeRound(h,bg,C(61,71,84),20);
    for(int i=0;i<6;i++){RECT b{x,y+i*(UI_SLOT+UI_GAP),x+UI_SLOT,y+i*(UI_SLOT+UI_GAP)+UI_SLOT};FillRound(h,b,g.slots[i].valid?C(34,42,54):C(29,36,46),12);if(g.slots[i].valid)Icon(h,g.slots[i].command==ID_TOOL_LINE?10:6,b,g.selectedTool==g.slots[i].command);else Icon(h,24,b,false);StrokeRound(h,b,C(60,72,89),12);}
    RECT star{x,y+6*(UI_SLOT+UI_GAP)+6,x+UI_SLOT,y+6*(UI_SLOT+UI_GAP)+6+UI_SLOT};FillRound(h,star,C(29,36,46),12);Icon(h,19,star,false);StrokeRound(h,star,C(60,72,89),12);
    RECT layer{x,y+7*(UI_SLOT+UI_GAP)+10,x+UI_SLOT,y+7*(UI_SLOT+UI_GAP)+10+UI_SLOT};FillRound(h,layer,g.layers?C(55,116,210):C(29,36,46),12);Icon(h,20,layer,g.layers);StrokeRound(h,layer,C(60,72,89),12);
    RECT settings{x,y+8*(UI_SLOT+UI_GAP)+14,x+UI_SLOT,y+8*(UI_SLOT+UI_GAP)+14+UI_SLOT};FillRound(h,settings,g.settings?C(55,116,210):C(29,36,46),12);Icon(h,21,settings,g.settings);StrokeRound(h,settings,C(60,72,89),12);
}
void DrawLayerPanel(HDC h,RECT rc){if(!g.layers)return;int right=rc.right-UI_RIGHT-18,left=right-UI_PANEL_W,bottom=rc.bottom-24,top=92;RECT p{left,top,right,bottom};FillRound(h,p,C(20,26,35),20);StrokeRound(h,p,C(70,80,95),20);Text(h,L"图层",RECT{left+24,top+14,left+120,top+48},20,C(245,248,252));Text(h,L"×",RECT{right-46,top+13,right-18,top+45},25,C(200,210,220),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    const wchar_t* names[]={L"重点标记",L"课前板书",L"课堂演算",L"学生答案",L"PDF",L"背景"};
    for(int i=0;i<6;i++){int y=top+62+i*58;RECT row{left+12,y,right-12,y+50};if(i==2)FillRound(h,row,C(47,103,195),10);Icon(h,i<4?6:20,RECT{left+28,y+7,left+56,y+35},i==2);Text(h,names[i],RECT{left+66,y,right-90,y+50},16,C(232,237,244));Icon(h,i<4?22:23,RECT{right-70,y+10,right-40,y+40},i<4);Icon(h,24,RECT{right-38,y+10,right-10,y+40},false);}
    int y=bottom-58;int xs[]={left+16,left+82,left+148,left+214,left+280};int icons[]={24,25,26,23,27};const wchar_t* labels[]={L"新建",L"上移",L"下移",L"锁定",L"删除"};for(int i=0;i<5;i++){RECT b{xs[i],y,xs[i]+52,y+44};FillRound(h,b,C(30,37,47),10);Icon(h,icons[i],RECT{b.left+12,b.top+5,b.left+40,b.top+33},false);Text(h,labels[i],RECT{b.left,b.top+25,b.right,b.bottom},11,C(170,180,192),DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
}
void DrawSettings(HDC h,RECT rc){if(!g.settings)return;int right=rc.right-UI_RIGHT-18,left=right-330,bottom=rc.bottom-24,top=rc.bottom-320;RECT p{left,top,right,bottom};FillRound(h,p,C(20,26,35),20);StrokeRound(h,p,C(70,80,95),20);Text(h,L"页面设置",RECT{left+22,top+14,left+150,top+46},19,C(245,248,252));Text(h,L"×",RECT{right-44,top+12,right-18,top+42},25,C(200,210,220),DT_CENTER|DT_VCENTER|DT_SINGLELINE);Text(h,L"换页方式",RECT{left+22,top+60,right-20,top+90},14,C(180,190,202));
    RECT a{left+22,top+98,left+150,top+164},b{left+162,top+98,right-22,top+164};FillRound(h,a,g.singlePage?C(55,116,210):C(30,37,47),12);FillRound(h,b,!g.singlePage?C(55,116,210):C(30,37,47),12);Icon(h,4,RECT{a.left+14,a.top+12,a.left+48,a.top+46},g.singlePage);Icon(h,4,RECT{b.left+14,b.top+12,b.left+48,b.top+46},!g.singlePage);Text(h,L"单页（左右切换）",RECT{a.left+50,a.top+12,a.right-6,a.bottom-10},12,C(235,240,246));Text(h,L"连续（上下滚动）",RECT{b.left+50,b.top+12,b.right-6,b.bottom-10},12,C(235,240,246));
    Text(h,L"两侧显示背景",RECT{left+22,top+184,right-90,top+220},14,C(220,226,234));Text(h,g.background?L"●":L"○",RECT{right-62,top+178,right-24,top+224},26,g.background?C(70,140,240):C(130,140,150),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    Text(h,L"显示笔记",RECT{left+22,top+232,right-90,top+268},14,C(220,226,234));Text(h,g.notes?L"●":L"○",RECT{right-62,top+226,right-24,top+272},26,g.notes?C(70,140,240):C(130,140,150),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

LRESULT CALLBACK Proc(HWND w,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_LBUTTONDOWN:{POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};RECT rc=ClientRect();
        if(p.y<=70){
            int index=(p.x-18)/46;if(p.x>=18&&index>=0){
                if(index==0){Send(ID_OPEN);return 0;}if(index==1){Send(ID_SAVE);return 0;}if(index==2){Send(ID_ZOOM_OUT);return 0;}if(index==3){Send(ID_ZOOM_IN);return 0;}if(index==4){Send(ID_FIT_WIDTH);return 0;}if(index==5){Send(ID_PAN);return 0;}
                if(index>=7&&index<=11){int ids[]={ID_TOOL_PEN,ID_TOOL_RULER,ID_TOOL_LASSO,ID_TOOL_ERASER,ID_TOOL_LINE};g.selectedTool=ids[index-7];Send(g.selectedTool);InvalidateUi();return 0;}
            }
            int propX=18+12*46+12;int cids[]={ID_PEN_BLACK,ID_PEN_RED,ID_PEN_BLUE};for(int i=0;i<3;i++){RECT b{propX+i*42,14,propX+i*42+38,52};if(Hit(p,b)){g.color=cids[i];Send(g.color);InvalidateUi();return 0;}}
            propX+=3*42+4;int wids[]={ID_WIDTH_THIN,ID_WIDTH_MEDIUM,ID_WIDTH_THICK};for(int i=0;i<3;i++){RECT b{propX+i*42,14,propX+i*42+38,52};if(Hit(p,b)){g.width=wids[i];Send(g.width);InvalidateUi();return 0;}}
            propX+=3*42+4;if(Hit(p,RECT{propX,14,propX+38,52})){g.dashed=!g.dashed;Send(ID_DASH);InvalidateUi();return 0;}propX+=44;if(Hit(p,RECT{propX,14,propX+38,52})){g.oneStroke=!g.oneStroke;Send(ID_ONE_STROKE);InvalidateUi();return 0;}
            return 0;
        }
        int x=rc.right-UI_RIGHT-4,y=86;for(int i=0;i<6;i++){RECT b{x,y+i*(UI_SLOT+UI_GAP),x+UI_SLOT,y+i*(UI_SLOT+UI_GAP)+UI_SLOT};if(Hit(p,b)){ActivateSlot(i);return 0;}}
        RECT star{x,y+6*(UI_SLOT+UI_GAP)+6,x+UI_SLOT,y+6*(UI_SLOT+UI_GAP)+6+UI_SLOT};if(Hit(p,star)){AddFavorite();return 0;}
        RECT layer{x,y+7*(UI_SLOT+UI_GAP)+10,x+UI_SLOT,y+7*(UI_SLOT+UI_GAP)+10+UI_SLOT};if(Hit(p,layer)){g.layers=!g.layers;g.settings=false;InvalidateUi();return 0;}
        RECT settings{x,y+8*(UI_SLOT+UI_GAP)+14,x+UI_SLOT,y+8*(UI_SLOT+UI_GAP)+14+UI_SLOT};if(Hit(p,settings)){g.settings=!g.settings;g.layers=false;InvalidateUi();return 0;}
        if(g.layers){int right=rc.right-UI_RIGHT-18,left=right-UI_PANEL_W,top=92;if(Hit(p,RECT{right-52,top+8,right-10,top+52})){g.layers=false;InvalidateUi();return 0;}}
        if(g.settings){int right=rc.right-UI_RIGHT-18,left=right-330,top=rc.bottom-320;if(Hit(p,RECT{right-48,top+8,right-8,top+50})){g.settings=false;InvalidateUi();return 0;}if(Hit(p,RECT{left+22,top+98,left+150,top+164})){g.singlePage=true;InvalidateUi();return 0;}if(Hit(p,RECT{left+162,top+98,right-22,top+164})){g.singlePage=false;InvalidateUi();return 0;}if(Hit(p,RECT{right-80,top+170,right-15,top+230})){g.background=!g.background;if(g.main)SendMessageW(g.main,WM_MOSUAN_BG_VIS,g.background,0);InvalidateUi();return 0;}if(Hit(p,RECT{right-80,top+220,right-15,top+280})){g.notes=!g.notes;if(g.main)SendMessageW(g.main,WM_MOSUAN_NOTE_VIS,g.notes,0);InvalidateUi();return 0;}}
        return 0;}
    case WM_NCHITTEST:{POINT p{};GetCursorPos(&p);ScreenToClient(w,&p);RECT rc=ClientRect();int x=rc.right-UI_RIGHT-4;if(p.y<=70||p.x>=x-8)return HTCLIENT;if(g.layers||g.settings)return HTCLIENT;return HTTRANSPARENT;}
    case WM_PAINT:{PAINTSTRUCT ps{};HDC h=BeginPaint(w,&ps);RECT rc{};GetClientRect(w,&rc);DrawTop(h,rc);DrawFavoriteBar(h,rc);DrawLayerPanel(h,rc);DrawSettings(h,rc);EndPaint(w,&ps);return 0;}
    case WM_ERASEBKGND:return 1;
    case WM_SIZE:InvalidateRect(w,nullptr,FALSE);return 0;
    }
    return DefWindowProcW(w,msg,wp,lp);
}

void HideLegacy(HWND main){EnumChildWindows(main,[](HWND h,LPARAM)->BOOL{wchar_t cls[32]{};GetClassNameW(h,cls,32);if(lstrcmpW(cls,L"BUTTON")==0||lstrcmpW(cls,L"STATIC")==0)ShowWindow(h,SW_HIDE);return TRUE;},0);}
DWORD WINAPI Thread(void*){for(int i=0;i<100;i++){g.main=FindWindowW(kMainClass,nullptr);if(g.main)break;Sleep(100);}if(!g.main)return 0;HideLegacy(g.main);HINSTANCE hi=GetModuleHandleW(nullptr);WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=hi;wc.lpfnWndProc=Proc;wc.lpszClassName=kUiClass;wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);wc.hbrBackground=nullptr;RegisterClassExW(&wc);g.ui=CreateWindowExW(0,kUiClass,L"",WS_CHILD|WS_VISIBLE,0,0,1000,1000,g.main,nullptr,hi,nullptr);while(IsWindow(g.main)){RECT r{};GetClientRect(g.main,&r);SetWindowPos(g.ui,HWND_TOP,0,0,r.right,r.bottom,SWP_NOACTIVATE|SWP_SHOWWINDOW);InvalidateRect(g.ui,nullptr,FALSE);Sleep(200);}return 0;}
struct Start{Start(){CreateThread(nullptr,0,Thread,nullptr,0,nullptr);}} start;
}
