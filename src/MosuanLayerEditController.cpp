#include "MosuanLayerEditController.h"
#include "AppWindow.h"

void MosuanLayerEditController::BeginRename(std::size_t index) {
    renaming_ = true;
    index_ = index;
    renameText_.clear();
}

void MosuanLayerEditController::SetRenameText(const std::wstring& text) {
    renameText_ = text;
}

bool MosuanLayerEditController::CommitRename(AppWindow& app) {
    if (!renaming_) return false;
    if (!renameText_.empty()) {
        // Layer panel will map index to layer id.
    }
    renaming_ = false;
    renameText_.clear();
    app.Refresh();
    return true;
}

void MosuanLayerEditController::CancelRename() {
    renaming_ = false;
    renameText_.clear();
}

void MosuanLayerEditController::BeginDrag(std::size_t index, int y) {
    dragging_ = true;
    index_ = index;
    startY_ = y;
    currentY_ = y;
}

bool MosuanLayerEditController::UpdateDrag(int y) {
    if (!dragging_) return false;
    currentY_ = y;
    return true;
}

bool MosuanLayerEditController::EndDrag(AppWindow& app) {
    if (!dragging_) return false;
    dragging_ = false;
    app.Refresh();
    return true;
}
