#include "MosuanUiTopBar.h"
#include "MosuanUiState.h"
#include "MosuanUiPrimitives.h"
namespace MosuanUI {
static RECT button(int i){int x=18+i*46;return {x,14,x+38,52};}
static void separator(HDC dc,int x){HPEN p=CreatePen(PS_SOLID,1,color(78,87,101));auto op=SelectObject(dc,p);line(dc,x,17,x,55);SelectObject(dc,op);DeleteObject(p);}
void drawTopBar(HDC dc,const RECT& rc){auto&s=state();fillRound(dc,{10,8,rc.right-10,64},color(24,30,39),28);strokeRound(dc,{10,8,rc.right-10,64},color(63,73,87),28);
int ids[]={ID_OPEN,ID_SAVE,ID_ZOOM_OUT,ID_ZOOM_IN,ID_FIT_WIDTH,ID_PAN};int icons[]={0,1,2,3,4,5};
for(int i=0;i<6;i++){RECT b=button(i);bool a=ids[i]==ID_PAN;fillRound(dc,b,a?color(55,116,210):color(31,38,48),10);icon(dc,icons[i],b,a);}separator(dc,18+6*46-4);
int start=7;int tools[]={ID_TOOL_PEN,ID_TOOL_RULER,ID_TOOL_LASSO,ID_TOOL_ERASER,ID_TOOL_LINE};int toolIcons[]={6,7,8,9,10};
for(int i=0;i<5;i++){RECT b=button(start+i);bool a=s.selectedTool==tools[i];fillRound(dc,b,a?color(55,116,210):color(31,38,48),10);icon(dc,toolIcons[i],b,a);}separator(dc,18+(start+5)*46-4);
int x=18+(start+5)*46+12;int colors[]={ID_PEN_BLACK,ID_PEN_RED,ID_PEN_BLUE};COLORREF dots[]={color(15,17,20),color(238,55,70),color(65,100,235)};
for(int i=0;i<3;i++){RECT b{x,14,x+38,52};if(s.color==colors[i])fillRound(dc,b,color(40,48,60),10);circle(dc,(b.left+b.right)/2,(b.top+b.bottom)/2,8,dots[i]);x+=42;}x+=4;
int widths[]={ID_WIDTH_THIN,ID_WIDTH_MEDIUM,ID_WIDTH_THICK};int wi[]={14,15,16};for(int i=0;i<3;i++){RECT b{x,14,x+38,52};if(s.width==widths[i])fillRound(dc,b,color(40,48,60),10);icon(dc,wi[i],b,s.width==widths[i]);x+=42;}x+=4;
RECT dash{x,14,x+38,52};if(s.dashed)fillRound(dc,dash,color(55,116,210),10);icon(dc,17,dash,s.dashed);x+=44;RECT one{x,14,x+38,52};if(s.oneStroke)fillRound(dc,one,color(55,116,210),10);icon(dc,18,one,s.oneStroke);
}
}
