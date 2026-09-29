#pragma once
#include <windows.h>
namespace MosuanUI {
COLORREF color(int r,int g,int b);
void fillRound(HDC dc,const RECT& r,COLORREF fill,int radius=14);
void strokeRound(HDC dc,const RECT& r,COLORREF stroke,int radius=14);
void line(HDC dc,int x1,int y1,int x2,int y2);
void circle(HDC dc,int x,int y,int radius,COLORREF fill);
void text(HDC dc,const wchar_t* value,const RECT& r,int size=15,COLORREF fg=RGB(230,235,242),UINT flags=DT_LEFT|DT_VCENTER|DT_SINGLELINE);
void icon(HDC dc,int kind,const RECT& r,bool active=false);
}
