#pragma once
#include "FavoriteTool.h"
#include "History.h"
#include "ToolState.h"

// Shared application state used by both desktop front ends.
class MosuanState {
public:
    ToolState& Tools() { return tools_; }
    const ToolState& Tools() const { return tools_; }

    FavoriteToolStore& Favorites() { return favorites_; }
    const FavoriteToolStore& Favorites() const { return favorites_; }

    History& HistoryStack() { return history_; }
    const History& HistoryStack() const { return history_; }

    bool SaveCurrentAsFavorite() { return favorites_.Add(tools_.brush); }

    bool ActivateFavorite(std::size_t index) {
        const FavoriteTool* favorite = favorites_.Get(index);
        if (!favorite || !favorite->occupied) return false;
        tools_.brush = favorite->state;
        tools_.activeTool = favorite->state.tool;
        tools_.lassoActive = favorite->state.tool == MosuanTool::Lasso;
        tools_.eraserActive = favorite->state.tool == MosuanTool::Eraser;
        return true;
    }

private:
    ToolState tools_{};
    FavoriteToolStore favorites_{};
    History history_{};
};
