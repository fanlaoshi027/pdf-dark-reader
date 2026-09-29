#include "InkToolState.h"
#include <algorithm>

void InkToolState::Reset() {
    tool_ = MosuanTool::Pen;
    colorIndex_ = 0;
    widthIndex_ = 1;
    dashed_ = false;
    oneStroke_ = true;
    for (auto& slot : slots_) slot = {};
}

void InkToolState::SetColorIndex(int index) noexcept { colorIndex_ = std::clamp(index, 0, 2); }
void InkToolState::SetWidthIndex(int index) noexcept { widthIndex_ = std::clamp(index, 0, 2); }

COLORREF InkToolState::Color() const noexcept {
    switch (colorIndex_) {
    case 1: return RGB(220, 55, 55);
    case 2: return RGB(45, 90, 210);
    default: return RGB(35, 35, 35);
    }
}

float InkToolState::Width() const noexcept {
    switch (widthIndex_) {
    case 0: return 2.0f;
    case 2: return 7.0f;
    default: return 4.0f;
    }
}

StrokeStyle InkToolState::Style() const noexcept {
    return StrokeStyle{Color(), Width(), dashed_, oneStroke_};
}

void InkToolState::SaveSlot(std::size_t slot) noexcept {
    if (slot >= slots_.size()) return;
    slots_[slot] = Slot{colorIndex_, widthIndex_, dashed_, oneStroke_};
}

void InkToolState::LoadSlot(std::size_t slot) noexcept {
    if (slot >= slots_.size()) return;
    colorIndex_ = slots_[slot].color;
    widthIndex_ = slots_[slot].width;
    dashed_ = slots_[slot].dashed;
    oneStroke_ = slots_[slot].oneStroke;
}
