#include "MosuanLayerPanel.h"
#include "AppWindow.h"
#include <algorithm>
#include <cwchar>

namespace {
constexpr int kClose=1,kAdd=2,kSelect=3,kPdf=4,kBackground=5,kToggleVisible=6,kToggleLock=7,kDelete=8,kRename=9,kUp=10,kDown=11;
constexpr int kFirstNoteY=158,kRowH=44,kGap=4;
}

bool MosuanLayerPanel::Paint(AppWindow& app,HDC hdc,const RECT& client){
 const int left=(std::max)(0,client.right-kWidth); RECT p{left,0,client.right,client.bottom};
 HBRUSH b=CreateSolidBrush(RGB(25,27,32));FillRect(hdc,&p,b);DeleteObject(b);
 SetBkMode(hdc,TRANSPARENT);SetTextColor(hdc,RGB(225,228,235));
 RECT title{left+18,10,client.right-42,48};DrawTextW(hdc,L"图层",-1,&title,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
 RECT close{client.right-40,8,client.right-10,44};DrawTextW(hdc,L"×",-1,&close,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
 auto row=[&](int y,const LayerItem& layer,bool active){
   RECT r{left+10,y,client.right-10,y+44};
   HBRUSH rb=CreateSolidBrush(active?RGB(52,57,67):RGB(31,34,40));FillRect(hdc,&r,rb);DeleteObject(rb);
   SetTextColor(hdc,layer.visible?RGB(205,210,218):RGB(100,104,112));
   RECT t{r.left+38,r.top,r.right-122,r.bottom};DrawTextW(hdc,layer.name,-1,&t,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
   RECT eye{r.right-118,r.top,r.right-96,r.bottom};DrawTextW(hdc,layer.visible?L"●":L"○",-1,&eye,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
   RECT lock{r.right-94,r.top,r.right-72,r.bottom};DrawTextW(hdc,layer.locked?L"■":L"□",-1,&lock,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
   if(layer.kind==LayerKind::Ink && layer.id>3){
      RECT up{r.right-70,r.top,r.right-50,r.bottom};DrawTextW(hdc,L"↑",-1,&up,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
      RECT down{r.right-50,r.top,r.right-30,r.bottom};DrawTextW(hdc,L"↓",-1,&down,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
      RECT del{r.right-28,r.top,r.right-4,r.bottom};SetTextColor(hdc,RGB(170,130,135));DrawTextW(hdc,L"×",-1,&del,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
   }
 };
 LayerItem bg{1,LayerKind::Background,L"背景层",app.layers().BackgroundVisible(),false};
 LayerItem pdf{2,LayerKind::Pdf,L"PDF",app.layers().PdfVisible(),false};
 row(62,bg,false); row(110,pdf,false);
 int y=kFirstNoteY;
 for(const auto& layer:app.layers().Layers()) if(layer.kind==LayerKind::Ink){row(y,layer,layer.id==app.layers().ActiveLayerId());y+=kRowH+kGap;}
 RECT add{left+10,y+6,client.right-10,y+48};HBRUSH ab=CreateSolidBrush(RGB(34,38,45));FillRect(hdc,&add,ab);DeleteObject(ab);SetTextColor(hdc,RGB(180,220,255));DrawTextW(hdc,L"＋  新建笔记层",-1,&add,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
 return true;
}

bool MosuanLayerPanel::HitTest(const RECT&c,int x,int y,int&a,std::size_t&i){
 const int l=c.right-kWidth;if(x<l||x>=c.right)return false;i=0;a=0;
 if(y<52){a=kClose;return true;} if(y>=62&&y<106){a=kBackground;return true;} if(y>=110&&y<154){a=kPdf;return true;}
 int index=(y-kFirstNoteY)/(kRowH+kGap);
 if(index>=0){int rowY=kFirstNoteY+index*(kRowH+kGap);if(y>=rowY&&y<rowY+kRowH){int rel=x-l-10;
     if(rel>=230){a=kDelete;}
     else if(rel>=210){a=kDown;}
     else if(rel>=190){a=kUp;}
     else if(rel>=166){a=kToggleLock;}
     else if(rel>=142){a=kToggleVisible;}
     else a=kSelect;
     i=static_cast<std::size_t>(index);return true;}}
 int noteCount=0;for(const auto&dummy:app_dummy_layers_){(void)dummy;++noteCount;}
 int addY=kFirstNoteY+noteCount*(kRowH+kGap);if(y>=addY&&y<addY+54){a=kAdd;return true;}
 return false;
}

void MosuanLayerPanel::Execute(AppWindow&app,int a,std::size_t index){
 if(a==kClose){app.SetLayerPanelOpen(false);}
 else if(a==kBackground){app.layers().SetBackgroundVisible(!app.layers().BackgroundVisible());}
 else if(a==kPdf){app.layers().SetPdfVisible(!app.layers().PdfVisible());}
 else if(a==kAdd){app.layers().CreateNoteLayer(L"笔记层");app.ApplyInkState();}
 else {size_t n=0;int id=-1;for(const auto&layer:app.layers().Layers())if(layer.kind==LayerKind::Ink){if(n++==index){id=layer.id;break;}}
   if(id>0){if(a==kSelect){app.layers().SetActiveLayer(id);app.ApplyInkState();}
   else if(a==kToggleVisible){app.layers().SetLayerVisible(id,!app.layers().IsLayerVisible(id));}
   else if(a==kToggleLock){app.layers().SetLayerLocked(id,!app.layers().IsLayerLocked(id));}
   else if(a==kDelete){app.layers().RemoveLayer(id);app.ApplyInkState();}
   else if(a==kUp){app.layers().MoveLayerUp(id);}
   else if(a==kDown){app.layers().MoveLayerDown(id);}}
 }
 app.Refresh();
}
