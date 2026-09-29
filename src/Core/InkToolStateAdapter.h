#pragma once
#include <algorithm>
#include "MosuanState.h"

// Compatibility bridge while the Windows UI migrates away from InkToolState.
// New code should use MosuanState directly.
class InkToolStateAdapter {
public:
    explicit InkToolStateAdapter(MosuanState& state) : state_(state) {}

    MosuanTool Tool() const { return state_.Tools().activeTool; }
    void SetTool(MosuanTool tool) { state_.Tools().activeTool = tool; state_.Tools().brush.tool = tool; }

    int ColorIndex() const {
        switch (state_.Tools().brush.color) {
        case 0x003737DCu: return 1;
        case 0x00D25A2Du: return 2;
        default: return 0;
        }
    }
    void SetColorIndex(int index) {
        index = (std::max)(0, (std::min)(2, index));
        static constexpr uint32_t colors[] = {0x00232323u, 0x003737DCu, 0x00D25A2Du};
        state_.Tools().brush.color = colors[index];
    }

    int WidthIndex() const {
        const float width = state_.Tools().brush.width;
        if (width <= 2.5f) return 0;
        if (width >= 5.5f) return 2;
        return 1;
    }
    void SetWidthIndex(int index) {
        index = (std::max)(0, (std::min)(2, index));
        static constexpr float widths[] = {2.0f, 4.0f, 7.0f};
        state_.Tools().brush.width = widths[index];
    }

    bool Dashed() const { return state_.Tools().brush.dashed; }
    void SetDash(bool value) { state_.Tools().brush.dashed = value; }

    bool OneStroke() const { return state_.Tools().brush.oneStroke; }
    void SetOneStroke(bool value) { state_.Tools().brush.oneStroke = value; }

    bool SaveSlot(std::size_t slot) { return state_.SaveCurrentToSlot(slot); }
    bool LoadSlot(std::size_t slot) { return state_.ActivateFavorite(slot); }

private:
    MosuanState& state_;
};
