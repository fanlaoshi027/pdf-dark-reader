#pragma once
#include <string>
#include <vector>

namespace mosuan::ui {

enum class LayerKind { Background, Pdf, Notes };

struct UiLayer {
    std::string name;
    LayerKind kind = LayerKind::Notes;
    bool visible = true;
    bool locked = false;
};

class LayerPanelModel {
public:
    LayerPanelModel() {
        layers_.push_back({"背景", LayerKind::Background, true, true});
        layers_.push_back({"PDF", LayerKind::Pdf, true, true});
        layers_.push_back({"笔记层 1", LayerKind::Notes, true, false});
    }

    const std::vector<UiLayer>& layers() const noexcept { return layers_; }
    int currentIndex() const noexcept { return currentIndex_; }
    void select(int index) noexcept {
        if (index >= 0 && index < static_cast<int>(layers_.size())) currentIndex_ = index;
    }
    void addNotesLayer(const std::string& name) {
        layers_.push_back({name, LayerKind::Notes, true, false});
        currentIndex_ = static_cast<int>(layers_.size()) - 1;
    }
    void toggleVisible(int index) noexcept {
        if (index >= 0 && index < static_cast<int>(layers_.size())) layers_[index].visible = !layers_[index].visible;
    }
    void toggleLocked(int index) noexcept {
        if (index >= 0 && index < static_cast<int>(layers_.size())) layers_[index].locked = !layers_[index].locked;
    }

private:
    std::vector<UiLayer> layers_;
    int currentIndex_ = 2;
};

} // namespace mosuan::ui
