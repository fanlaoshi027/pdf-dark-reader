#pragma once
#include <windows.h>
#include <array>
#include <cstddef>
#include "LayerSystem.h"
#include "StrokeDynamics.h"

class InkToolState {
public:
    void Reset();
    void SetTool(MosuanTool tool) noexcept { tool_ = tool; }
    MosuanTool Tool() const noexcept { return tool_; }
    void SetColorIndex(int index) noexcept;
    void SetWidthIndex(int index) noexcept;
    void ToggleDash() noexcept { dashed_ = !dashed_; }
    void ToggleOneStroke() noexcept { oneStroke_ = !oneStroke_; }
    void SetDash(bool value) noexcept { dashed_ = value; }
    void SetOneStroke(bool value) noexcept { oneStroke_ = value; }
    int ColorIndex() const noexcept { return colorIndex_; }
    int WidthIndex() const noexcept { return widthIndex_; }
    bool Dashed() const noexcept { return dashed_; }
    bool OneStroke() const noexcept { return oneStroke_; }
    COLORREF Color() const noexcept;
    float Width() const noexcept;
    StrokeStyle Style() const noexcept;

    void SaveSlot(std::size_t slot) noexcept;
    void LoadSlot(std::size_t slot) noexcept;
    static constexpr std::size_t kSlotCount = 8;

private:
    struct Slot { int color = 0; int width = 1; bool dashed = false; bool oneStroke = true; };
    MosuanTool tool_ = MosuanTool::Pen;
    int colorIndex_ = 0;
    int widthIndex_ = 1;
    bool dashed_ = false;
    bool oneStroke_ = true;
    std::array<Slot, kSlotCount> slots_{};
};
