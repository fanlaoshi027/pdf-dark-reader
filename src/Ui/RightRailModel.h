#pragma once
#include <array>

namespace mosuan::ui {

struct FavoriteSlot {
    bool occupied = false;
    int tool = 0;
};

class RightRailModel {
public:
    static constexpr int kFavoriteCount = 6;

    const FavoriteSlot& favorite(int index) const noexcept { return slots_[index]; }
    void store(int index, int tool) noexcept {
        if (index < 0 || index >= kFavoriteCount) return;
        slots_[index] = {true, tool};
    }
    bool layerPanelOpen() const noexcept { return layerPanelOpen_; }
    bool settingsOpen() const noexcept { return settingsOpen_; }
    void toggleLayerPanel() noexcept { layerPanelOpen_ = !layerPanelOpen_; settingsOpen_ = false; }
    void toggleSettings() noexcept { settingsOpen_ = !settingsOpen_; layerPanelOpen_ = false; }

private:
    std::array<FavoriteSlot, kFavoriteCount> slots_{};
    bool layerPanelOpen_ = false;
    bool settingsOpen_ = false;
};

} // namespace mosuan::ui
