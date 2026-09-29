#include "LayerSystem.h"
#include <cwchar>

void LayerSystem::ResetDocumentLayers() {
    layers_.clear();

    LayerItem background;
    background.id = 1;
    background.kind = LayerKind::Background;
    std::wcscpy(background.name, L"背景");
    layers_.push_back(background);

    LayerItem pdf;
    pdf.id = 2;
    pdf.kind = LayerKind::Pdf;
    std::wcscpy(pdf.name, L"PDF");
    layers_.push_back(pdf);

    LayerItem ink;
    ink.id = 3;
    ink.kind = LayerKind::Ink;
    std::wcscpy(ink.name, L"笔记");
    layers_.push_back(ink);

    activeLayerId_ = 3;
    nextLayerId_ = 4;
}

int LayerSystem::AddInkLayer(const wchar_t* name) {
    LayerItem item;
    item.id = nextLayerId_++;
    item.kind = LayerKind::Ink;
    if (name && *name) {
        std::wcsncpy(item.name, name, 63);
        item.name[63] = L'\0';
    } else {
        std::swprintf(item.name, 64, L"笔记 %d", item.id - 2);
    }
    layers_.push_back(item);
    activeLayerId_ = item.id;
    return item.id;
}

bool LayerSystem::RemoveLayer(int id) {
    if (id <= 3) return false;
    for (auto it = layers_.begin(); it != layers_.end(); ++it) {
        if (it->id == id) {
            layers_.erase(it);
            if (activeLayerId_ == id) activeLayerId_ = 3;
            return true;
        }
    }
    return false;
}

bool LayerSystem::SetActiveLayer(int id) {
    LayerItem* item = FindLayer(id);
    if (!item || !item->visible || item->locked) return false;
    activeLayerId_ = id;
    return true;
}

LayerItem* LayerSystem::FindLayer(int id) {
    for (auto& item : layers_) if (item.id == id) return &item;
    return nullptr;
}

void LayerSystem::SetLayerVisible(int id, bool visible) {
    if (auto* item = FindLayer(id)) item->visible = visible;
    if (id == activeLayerId_ && !visible) activeLayerId_ = 3;
}

void LayerSystem::SetLayerLocked(int id, bool locked) {
    if (auto* item = FindLayer(id)) item->locked = locked;
    if (id == activeLayerId_ && locked) activeLayerId_ = 3;
}
