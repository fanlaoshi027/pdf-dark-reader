#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>
#include <algorithm>
#include "PdfDocument.h"
#include "InvertSettings.h"
#include "LayerSystem.h"
#include "PdfViewState.h"
#include "PdfThumbnailRail.h"
#include "WindowsInkCanvas.h"

class AppWindow {
public:
    bool Create(HINSTANCE instance);
    int Run();
    void SelectPageFromThumbnail(int page) { if (!pdf_.IsOpen() || page < 0 || page >= pdf_.PageCount()) return; pageIndex_=page; scrollY_=0; inkCanvas_.Clear(); RenderCurrentPage(); Refresh(); }
    void Refresh() noexcept { InvalidateRect(hwnd_,nullptr,FALSE); PdfThumbnailRail::Refresh(hwnd_); }
    HWND hwnd() const noexcept { return hwnd_; }
    PdfDocument& pdf() noexcept { return pdf_; }
    const PdfDocument& pdf() const noexcept { return pdf_; }
    int pageIndex() const noexcept { return pageIndex_; }
    bool invertEnabled() const noexcept { return invert_; }
    const InvertSettings& invertSettings() const noexcept { return invertSettings_; }
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    void Paint(HDC hdc); void DrawToolbarButton(const DRAWITEMSTRUCT* dis); void OpenPdf(); void RenderCurrentPage(); void ApplyInvert(); void CreateToolbar(); void LayoutToolbar(int width); void UpdateScrollBar(); void UpdateToolbarText(); void ChangeZoom(double factor); void ScrollBy(int delta); void SetInvert(bool enabled); void GoPage(int delta); void ChooseBackground(); void FitPage(); void FitWidth(); void ShowLayerMenu(); void UpdateLayerGeometry(); void SetMosuanTool(MosuanTool tool); void LayoutInkCanvas();
    HWND hwnd_=nullptr; HINSTANCE instance_=nullptr; PdfDocument pdf_; LayerSystem layers_; WindowsInkCanvas inkCanvas_; std::vector<std::uint8_t> pixels_; PdfViewState pdfView_; int pageIndex_=0; int renderWidth_=0; int renderHeight_=0; int scrollY_=0; double zoom_=1.0; bool fitWidth_=false; bool invert_=false; bool dashMode_=false; bool oneStrokeMode_=true; int penColorIndex_=0; int penWidthIndex_=1; MosuanTool activeTool_=MosuanTool::Pen; InvertSettings invertSettings_;
    HWND openButton_=nullptr,saveButton_=nullptr,zoomOutButton_=nullptr,zoomLabel_=nullptr,zoomInButton_=nullptr,fitWidthButton_=nullptr,panButton_=nullptr,penButton_=nullptr,rulerButton_=nullptr,lassoButton_=nullptr,eraserButton_=nullptr,lineButton_=nullptr,colorBlackButton_=nullptr,colorRedButton_=nullptr,colorBlueButton_=nullptr,thinButton_=nullptr,mediumButton_=nullptr,thickButton_=nullptr,dashButton_=nullptr,oneStrokeButton_=nullptr,pageLabel_=nullptr; HFONT toolbarFont_=nullptr;
};
