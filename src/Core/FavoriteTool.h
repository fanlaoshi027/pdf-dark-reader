#pragma once
#include <array>
#include <cstddef>
#include "BrushState.h"

// A favorite is a complete brush/tool preset, not just a toolbar button.
struct FavoriteTool {
    BrushState state{};
    bool occupied = false;
};

class FavoriteToolStore {
public:
    static constexpr std::size_t kMaxSlots = 8;

    bool Add(const BrushState& state) {
        for (auto& slot : slots_) {
            if (!slot.occupied) {
                slot.state = state;
                slot.occupied = true;
                return true;
            }
        }
        return false;
    }

    bool Remove(std::size_t index) {
        if (index >= kMaxSlots || !slots_[index].occupied) return false;
        slots_[index] = FavoriteTool{};
        return true;
    }

    const FavoriteTool* Get(std::size_t index) const {
        if (index >= kMaxSlots) return nullptr;
        return &slots_[index];
    }

    FavoriteTool* Get(std::size_t index) {
        if (index >= kMaxSlots) return nullptr;
        return &slots_[index];
    }

    void Clear() {
        for (auto& slot : slots_) slot = FavoriteTool{};
    }

private:
    std::array<FavoriteTool, kMaxSlots> slots_{};
};
