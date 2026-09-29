#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>
#include <utility>
#include "PdfDocument.h"
#include "InvertSettings.h"
#include "LayerSystem.h"
#include "InkDocument.h"
#include "InkEngine.h"
#include "InkToolState.h"

class AppWindow {
public:
    bool Create(HINSTANCE instance);
    int Run();

    HWND hwnd() const noexcept { return hwnd_; }
    HINSTANCE instance() const noexcept { return instance_; }
    PdfDocument& pdf() noexcept { return pdf_; }
    LayerSystem& layers() noexcept { return layers_; }
    InkEngine& Ink() noexcept { return ink_; }
    InkToolState& InkState() noexcept { return inkState_; }
    const std::vector<std::uint8_t>& pixels() const noexcept { return pixels_; }
    int pageIndex() const noexcept { return pageIndex_; }
    int renderWidth() const noexcept { return renderWidth_; }
    int renderHeight() const noexcept { return renderHeight_; }
    int scrollY() const noexcept { return scrollY_; }
    double zoom() const noexcept { return zoom_; }
    bool fitWidth() const noexcept { return fitWidth_; }
    bool fitWidthEnabled() const noexcept { return fitWidth_; }
    bool invertEnabled() const noexcept { return invert_; }
    bool dashMode() const noexcept { return inkState_.Dashed(); }
    bool oneStrokeMode() const noexcept { return inkState_.OneStroke(); }
    int penColorIndex() const noexcept { return inkState_.ColorIndex(); }
    int penWidthIndex() const noexcept { return inkState_.WidthIndex(); }
    MosuanTool activeTool() const noexcept { return inkState_.Tool(); }
    InvertSettings& invertSettings() noexcept { return invertSettings_; }

    void SetPageIndex(int value) noexcept { pageIndex_ = value; ink_.SetDocument(&inkDocument_); inkDocument_.SetCurrentPage(value); ink_.Cancel(); }
    void SetRenderSize(int width, int height) noexcept { renderWidth_ = width; renderHeight_ = height; }
    void SetScrollY(int value) noexcept { scrollY_ = value; }
    void SetZoom(double value) noexcept { zoom_ = value; }
    void SetFitWidth(bool value) noexcept { fitWidth_ = value; }
    void SetInvertState(bool value) noexcept { invert_ = value; }
    void SetDashMode(bool value) noexcept { inkState_.SetDash(value); }
    void SetOneStrokeMode(bool value) noexcept { inkState_.SetOneStroke(value); }
    void SetPenColorIndex(int value) noexcept { inkState_.SetColorIndex(value); }
    void SetPenWidthIndex(int value) noexcept { inkState_.SetWidthIndex(value); }
    void SetActiveTool(MosuanTool value) noexcept { activeTool_ = value; inkState_.SetTool(value); ink_.SetTool(value); }
    void SetPixels(std::vector<std::uint8_t> value) { pixels_ = std::move(value); }

    void ApplyInkState() noexcept { ink_.SetTool(inkState_.Tool()); ink_.SetStyle(inkState_.Style()); ink_.SetLayer(layers_.ActiveLayerId()); }
    void SaveInkSlot(std::size_t slot) noexcept { inkState_.SaveSlot(slot); }
    void LoadInkSlot(std::size_t slot) noexcept { inkState_.LoadSlot(slot); ApplyInkState(); }

    void Refresh() noexcept { InvalidateRect(hwnd_, nullptr, FALSE); }
    void UpdateScrollBar();
    void UpdateToolbarText();
    void SyncInkTransform(int originX, int originY);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    PdfDocument pdf_;
    LayerSystem layers_;
    InkDocument inkDocument_;
    InkEngine ink_;
    InkToolState inkState_;
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
