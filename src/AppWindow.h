#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>
#include "PdfDocument.h"
#include "InvertSettings.h"
#include "LayerSystem.h"
#include "PdfViewState.h"

class AppWindow {
public:
    bool Create(HINSTANCE instance);
    int Run();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    void Paint(HDC hdc);
    void DrawToolbarButton(const DRAWITEMSTRUCT* dis);
    void OpenPdf();
    void RenderCurrentPage();
    void ApplyInvert();
    void CreateToolbar();
    void LayoutToolbar(int width);
    void UpdateScrollBar();
    void UpdateToolbarText();
    void ChangeZoom(double factor);
    void ScrollBy(int delta);
    void SetInvert(bool enabled);
    void GoPage(int delta);
    void ChooseBackground();
    void FitPage();
    void FitWidth();
    void ShowLayerMenu();
    void UpdateLayerGeometry();
    void SetMosuanTool(MosuanTool tool);

    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    PdfDocument pdf_;
    LayerSystem layers_;
    std::vector<std::uint8_t> pixels_;
    PdfViewState pdfView_;
    int renderWidth_ = 0;
    int renderHeight_ = 0;
    int scrollY_ = 0;
    double zoom_ = 1.0;
    bool fitWidth_ = false;
    bool invert_ = false;
    bool dashMode_ = false;
    bool oneStrokeMode_ = true;
    int penColorIndex_ = 0;
    int penWidthIndex_ = 1;
    MosuanTool activeTool_ = MosuanTool::Pen;
    InvertSettings invertSettings_;

    HWND openButton_ = nullptr;
    HWND saveButton_ = nullptr;
    HWND zoomOutButton_ = nullptr;
    HWND zoomLabel_ = nullptr;
    HWND zoomInButton_ = nullptr;
    HWND fitWidthButton_ = nullptr;
    HWND panButton_ = nullptr;
    HWND penButton_ = nullptr;
    HWND rulerButton_ = nullptr;
    HWND lassoButton_ = nullptr;
    HWND eraserButton_ = nullptr;
    HWND lineButton_ = nullptr;
    HWND colorBlackButton_ = nullptr;
    HWND colorRedButton_ = nullptr;
    HWND colorBlueButton_ = nullptr;
    HWND thinButton_ = nullptr;
    HWND mediumButton_ = nullptr;
    HWND thickButton_ = nullptr;
    HWND dashButton_ = nullptr;
    HWND oneStrokeButton_ = nullptr;
    HWND pageLabel_ = nullptr;
    HFONT toolbarFont_ = nullptr;
};