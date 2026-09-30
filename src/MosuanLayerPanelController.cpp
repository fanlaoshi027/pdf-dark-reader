#include "MosuanLayerPanelController.h"

MosuanLayerPanelController::MosuanLayerPanelController() = default;

void MosuanLayerPanelController::Attach(LayerSystem* system)
{
    layerSystem_ = system;
}

void MosuanLayerPanelController::Refresh()
{
    // UI refresh hook. The panel queries LayerSystem directly.
}

void MosuanLayerPanelController::HandleClick(const LayerPanelHitResult& hit)
{
    if (!layerSystem_) return;

    switch (hit.type)
    {
    case LayerPanelHitType::Layer:
        layerSystem_->SetActiveLayer(hit.layerId);
        break;
    case LayerPanelHitType::Visible:
        layerSystem_->SetLayerVisible(hit.layerId, !layerSystem_->IsLayerVisible(hit.layerId));
        break;
    case LayerPanelHitType::Lock:
        layerSystem_->SetLayerLocked(hit.layerId, !layerSystem_->IsLayerLocked(hit.layerId));
        break;
    case LayerPanelHitType::Delete:
        layerSystem_->RemoveLayer(hit.layerId);
        break;
    case LayerPanelHitType::Add:
        layerSystem_->CreateNoteLayer(L"新建笔记层");
        break;
    default:
        break;
    }
}
