#pragma once
#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <vector>
#include "Core/InkTypes.h"

struct LayerItem { int id=0; LayerKind kind=LayerKind::Ink; wchar_t name[64]=L"笔记"; bool visible=true; bool locked=false; };

class LayerSystem {
public:
 bool Create(HWND parent); void Resize(const RECT& viewport); void SetTransform(const PdfViewTransform& transform); const PdfViewTransform& Transform() const{return transform_;}
 void SetMosuanVisible(bool visible); bool MosuanVisible() const noexcept{return mosuanVisible_;} void SetMosuanActive(bool active){mosuanActive_=active;UpdateHitTest();} bool MosuanActive() const noexcept{return mosuanActive_;}
 void SetTool(MosuanTool tool); MosuanTool Tool() const noexcept{return tool_;} void SetPenEnabled(bool enabled); bool PenEnabled() const noexcept{return penEnabled_;}
 void SetPenColor(COLORREF color){penColor_=color;Invalidate();} COLORREF PenColor() const noexcept{return penColor_;} void SetPenWidth(float width){penWidth_=(std::max)(0.5f,width);Invalidate();} float PenWidth() const noexcept{return penWidth_;}
 void SetDashMode(bool dashed){dashMode_=dashed;Invalidate();} bool DashMode() const noexcept{return dashMode_;} void SetOneStrokeMode(bool enabled){oneStrokeMode_=enabled;}
 POINT PdfToView(double pdfX,double pdfY) const; POINT ViewToPdf(int viewX,int viewY) const;
 void ResetDocumentLayers(); int AddInkLayer(const wchar_t* name); int CreateNoteLayer(const wchar_t* name){return AddInkLayer(name);} bool RemoveLayer(int id); bool SetActiveLayer(int id); int ActiveLayerId() const noexcept{return activeLayerId_;} int MosuanLayerId() const noexcept{return activeLayerId_;}
 const std::vector<LayerItem>& Layers() const{return layers_;} LayerItem* FindLayer(int id); void SetLayerVisible(int id,bool visible); void SetLayerLocked(int id,bool locked);
 void RenameLayer(int id,const wchar_t* name); bool CanEditActiveLayer() const;
 bool MoveLayer(int id,int newIndex){
     auto it=std::find_if(layers_.begin(),layers_.end(),[&](const LayerItem& x){return x.id==id&&x.kind==LayerKind::Ink;});
     if(it==layers_.end()||it->locked)return false;
     int first=0; while(first<(int)layers_.size()&&layers_[first].kind!=LayerKind::Ink)++first;
     int last=(int)layers_.size()-1; newIndex=(std::max)(first,(std::min)(last,newIndex));
     LayerItem item=*it; int old=(int)std::distance(layers_.begin(),it); layers_.erase(it); if(newIndex>old)--newIndex;
     layers_.insert(layers_.begin()+newIndex,item); Invalidate(); return true;
 }
 bool MoveLayerUp(int id){
     auto it=std::find_if(layers_.begin(),layers_.end(),[&](const LayerItem& x){return x.id==id;});
     if(it==layers_.end()||it->kind!=LayerKind::Ink||it->locked)return false;
     for(auto p=it;p!=layers_.begin();){--p;if(p->kind==LayerKind::Ink){std::iter_swap(p,it);Invalidate();return true;}}
     return false;
 }
 bool MoveLayerDown(int id){
     auto it=std::find_if(layers_.begin(),layers_.end(),[&](const LayerItem& x){return x.id==id;});
     if(it==layers_.end()||it->kind!=LayerKind::Ink||it->locked)return false;
     for(auto p=std::next(it);p!=layers_.end();++p)if(p->kind==LayerKind::Ink){std::iter_swap(p,it);Invalidate();return true;}
     return false;
 }
 void SetBackgroundVisible(bool visible){backgroundVisible_=visible;Invalidate();} bool BackgroundVisible() const noexcept{return backgroundVisible_;} void SetPdfVisible(bool visible){pdfVisible_=visible;Invalidate();} bool PdfVisible() const noexcept{return pdfVisible_;}
 bool IsLayerVisible(int id) const; bool IsLayerLocked(int id) const;
 void SetActiveLayerId(int id){SetActiveLayer(id);} void ClearInk(); void PaintOverlay(HDC hdc);
private:
 struct InkLayerData { int layerId=0; std::vector<InkStroke> strokes; };
 static LRESULT CALLBACK OverlayProc(HWND,UINT,WPARAM,LPARAM); void UpdateHitTest(); void Invalidate(){if(overlay_)InvalidateRect(overlay_,nullptr,FALSE);}
 void BeginPen(UINT32,POINT,float); void UpdatePen(UINT32,POINT,float); void EndPen(UINT32); void EraseAt(POINT); void FinishLasso(); bool PointInLasso(const POINT&) const; bool StrokeSelected(const InkStroke&) const; InkLayerData* ActiveInkData(); const InkLayerData* ActiveInkData() const; static float PenWidthPdf(float); static float ClampPressure(float);
 HWND parent_=nullptr,overlay_=nullptr; PdfViewTransform transform_; bool mosuanVisible_=true,mosuanActive_=true,penEnabled_=true,backgroundVisible_=true,pdfVisible_=true; UINT32 activePointerId_=0; bool penDown_=false;
 std::vector<LayerItem> layers_; std::vector<InkLayerData> inkLayers_; int activeLayerId_=3,nextLayerId_=4; std::vector<POINT> lassoPoints_; std::vector<size_t> selectedStrokes_; MosuanTool tool_=MosuanTool::Pen; COLORREF penColor_=RGB(35,75,150); float penWidth_=2.0f; bool dashMode_=false,oneStrokeMode_=true;
};
