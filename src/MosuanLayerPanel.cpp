#include "MosuanLayerPanel.h"
#include "AppWindow.h"
#include <algorithm>

namespace { constexpr int kClose=1,kAdd=2,kSelect=3,kPdf=4,kBackground=5; }

bool MosuanLayerPanel::Paint(AppWindow& app,HDC hdc,const RECT& client){
 const int left=(std::max)(0,client.right-kWidth); RECT p{left,0,client.right,client.bottom};
 HBRUSH b=CreateSolidBrush(RGB(25,27,32));FillRect(hdc,&p,b);DeleteObject(b);
 SetBkMode(hdc,TRANSPARENT);SetTextColor(hdc,RGB(225,228,235));RECT title{left+18,10,client.right-42,48};DrawTextW(hdc,L"图层",-1,&title,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
 RECT close{client.right-40,8,client.right-10,44};DrawTextW(hdc,L"×",-1,&close,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
 auto row=[&](int y,const wchar_t*n,bool active,bool visible){RECT r{left+10,y,client.right-10,y+44};HBRUSH rb=CreateSolidBrush(active?RGB(52,57,67):RGB(31,34,40));FillRect(hdc,&r,rb);DeleteObject(rb);SetTextColor(hdc,RGB(185,190,198));RECT t{r.left+38,r.top,r.right-8,r.bottom};DrawTextW(hdc,n,-1,&t,DT_LEFT|DT_VCENTER|DT_SINGLELINE);SetTextColor(hdc,visible?RGB(150,205,255):RGB(75,80,88));RECT e{r.left+8,r.top,r.left+30,r.bottom};DrawTextW(hdc,visible?L"●":L"○",-1,&e,DT_CENTER|DT_VCENTER|DT_SINGLELINE);};
 row(62,L"背景层",false,app.layers().BackgroundVisible()); row(110,L"PDF",false,app.layers().PdfVisible()); row(158,L"笔记层",true,app.layers().MosuanVisible());
 RECT add{left+10,218,client.right-10,260};HBRUSH ab=CreateSolidBrush(RGB(34,38,45));FillRect(hdc,&add,ab);DeleteObject(ab);SetTextColor(hdc,RGB(180,220,255));DrawTextW(hdc,L"＋  新建笔记层",-1,&add,DT_CENTER|DT_VCENTER|DT_SINGLELINE);return true;
}
bool MosuanLayerPanel::HitTest(const RECT&c,int x,int y,int&a,std::size_t&i){int l=c.right-kWidth;if(x<l||x>=c.right)return false;i=0;a=0;if(y<52){a=kClose;return true;}if(y>=62&&y<106){a=kBackground;return true;}if(y>=110&&y<154){a=kPdf;return true;}if(y>=158&&y<202){a=kSelect;return true;}if(y>=218&&y<260){a=kAdd;return true;}return false;}
void MosuanLayerPanel::Execute(AppWindow&app,int a,std::size_t){switch(a){case kClose:app.SetLayerPanelOpen(false);break;case kBackground:app.layers().SetBackgroundVisible(!app.layers().BackgroundVisible());break;case kPdf:app.layers().SetPdfVisible(!app.layers().PdfVisible());break;case kSelect:app.layers().SetActiveLayer(app.layers().MosuanLayerId());app.ApplyInkState();break;case kAdd:app.layers().CreateNoteLayer(L"笔记层");app.layers().SetActiveLayer(app.layers().ActiveLayerId());app.ApplyInkState();break;}app.Refresh();}
