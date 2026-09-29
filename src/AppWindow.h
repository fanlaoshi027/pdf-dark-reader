#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>
#include <utility>
#include "PdfDocument.h"
#include "InvertSettings.h"
#include "LayerSystem.h"

class AppWindow {
public:
    bool Create(HINSTANCE instance);
    int Run();

    HWND hwnd() const noexcept { return hwnd_; }
    HINSTANCE instance() const noexcept { return instance_; }
    PdfDocument& pdf() noexcept { return pdf_; }
    LayerSystem& layers() noexcept { return layers_; }
    const std::vector<std::uint8_t>& pixels() const noexcept { return pixels_; }
    int pageIndex() const noexcept { return pageIndex_; }
    int renderWidth() const noexcept { return renderWidth_; }
    int renderHeight() const noexcept { return renderHeight_; }
    int scrollY() const noexcept { return scrollY_; }
    double zoom() const noexcept { return zoom_; }
    bool fitWidth() const noexcept { return fitWidth_; }
    bool fitWidthEnabled() const noexcept { return fitWidth_; }
    bool invertEnabled() const noexcept { return invert_; }
    bool dashMode() const noexcept { return dashMode_; }
    bool oneStrokeMode() const noexcept { return oneStrokeMode_; }
    int penColorIndex() const noexcept { return penColorIndex_; }
    int penWidthIndex() const noexcept { return penWidthIndex_; }
    MosuanTool activeTool() const noexcept { return activeTool_; }
    InvertSettings& invertSettings() noexcept { return invertSettings_; }

    void SetPageIndex(int value) noexcept { pageIndex_ = value; }
    void SetRenderSize(int width, int height) noexcept { renderWidth_ = width; renderHeight_ = height; }
    void SetScrollY(int value) noexcept { scrollY_ = value; }
    void SetZoom(double value) noexcept { zoom_ = value; }
    void SetFitWidth(bool value) noexcept { fitWidth_ = value; }
    void SetInvertState(bool value) noexcept { invert_ = value; }
    void SetDashMode(bool value) noexcept { dashMode_ = value; }
    void SetOneStrokeMode(bool value) noexcept { oneStrokeMode_ = value; }
    void SetPenColorIndex(int value) noexcept { penColorIndex_ = value; }
    void SetPenWidthIndex(int value) noexcept { penWidthIndex_ = value; }
    void SetActiveTool(MosuanTool value) noexcept { activeTool_ = value; }
    void SetPixels(std::vector<std::uint8_t> value) { pixels_ = std::move(value); }

    void Refresh() noexcept { InvalidateRect(hwnd_, nullptr, FALSE); }
    void UpdateScrollBar();
    void UpdateToolbarText();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    PdfDocument pdf_;
    LayerSystem layers_;
    std::vector<std::uint8_t> pixels_;
    int pageIndex_ = 0;
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
};
