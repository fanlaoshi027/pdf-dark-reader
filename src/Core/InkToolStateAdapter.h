#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include "MosuanState.h"

// Compatibility bridge while the Windows UI migrates away from InkToolState.
// New application code should use MosuanState directly.
class InkToolStateAdapter {
public:
    explicit InkToolStateAdapter(MosuanState& state) : state_(state) {}

    MosuanTool Tool() const { return state_.Tools().activeTool; }
    void SetTool(MosuanTool tool) { state_.Tools().activeTool = tool; state_.Tools().brush.tool = tool; }

    int ColorIndex() const {
        switch (state_.Tools().brush.color) {
        case kRed: return 1;
        case kBlue: return 2;
        default: return 0;
        }
    }
    void SetColorIndex(int index) {
        index = (std::max)(0, (std::min)(2, index));
        state_.Tools().brush.color = index == 1 ? kRed : (index == 2 ? kBlue : kBlack);
    }

    int WidthIndex() const {
        const float width = state_.Tools().brush.width;
        if (width >= 5.5f) return 2;
        if (width <= 3.0f) return 0;
        return 1;
    }
    void SetWidthIndex(int index) {
        index = (std::max)(0, (std::min)(2, index));
        state_.Tools().brush.width = index == 0 ? 2.0f : (index == 2 ? 7.0f : 4.0f);
    }

    bool Dashed() const { return state_.Tools().brush.dashed; }
    void SetDash(bool value) { state_.Tools().brush.dashed = value; }

    bool OneStroke() const { return state_.Tools().brush.oneStroke; }
    void SetOneStroke(bool value) { state_.Tools().brush.oneStroke = value; }

    bool SaveSlot(std::size_t slot) { return state_.SaveCurrentToSlot(slot); }
    bool LoadSlot(std::size_t slot) { return state_.ActivateFavorite(slot); }

private:
    static constexpr std::uint32_t kBlack = 0x00232323;
    static constexpr std::uint32_t kRed   = 0x00DC3737;
    static constexpr std::uint32_t kBlue  = 0x002D5AD2;
    MosuanState& state_;
};
