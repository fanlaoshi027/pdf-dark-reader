#pragma once
#include "MosuanState.h"

// Compatibility bridge while the Windows UI migrates away from InkToolState.
// New code should use MosuanState directly.
class InkToolStateAdapter {
public:
    explicit InkToolStateAdapter(MosuanState& state) : state_(state) {}

    MosuanTool Tool() const { return state_.Tools().activeTool; }
    void SetTool(MosuanTool tool) { state_.Tools().activeTool = tool; state_.Tools().brush.tool = tool; }

    bool Dashed() const { return state_.Tools().brush.dashed; }
    void SetDash(bool value) { state_.Tools().brush.dashed = value; }

    bool OneStroke() const { return state_.Tools().brush.oneStroke; }
    void SetOneStroke(bool value) { state_.Tools().brush.oneStroke = value; }

    bool SaveSlot(std::size_t slot) { return state_.SaveCurrentToSlot(slot); }
    bool LoadSlot(std::size_t slot) { return state_.ActivateFavorite(slot); }

private:
    MosuanState& state_;
};
