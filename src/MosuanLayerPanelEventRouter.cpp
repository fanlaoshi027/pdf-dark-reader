#include "MosuanLayerPanelEventRouter.h"

MosuanLayerPanelEventRouter::MosuanLayerPanelEventRouter() = default;

void MosuanLayerPanelEventRouter::Attach(MosuanLayerPanelController* controller,
                                         MosuanLayerPanelHitTest* hitTest)
{
    controller_ = controller;
    hitTest_ = hitTest;
}

void MosuanLayerPanelEventRouter::OnMouseDown(int x, int y)
{
    if (!hitTest_ || !controller_) return;

    auto target = hitTest_->HitTest(x, y);
    pressedTarget_ = target;
    dragging_ = target.type == LayerPanelHitType::LayerRow;

    if (target.type != LayerPanelHitType::None) {
        controller_->HandleHit(target);
    }
}

void MosuanLayerPanelEventRouter::OnMouseMove(int x, int y)
{
    if (!dragging_ || !controller_) return;

    controller_->UpdateDrag(x, y);
}

void MosuanLayerPanelEventRouter::OnMouseUp(int x, int y)
{
    if (dragging_ && controller_) {
        controller_->EndDrag(x, y);
    }

    dragging_ = false;
    pressedTarget_ = {};
}

void MosuanLayerPanelEventRouter::OnDoubleClick(int x, int y)
{
    if (!hitTest_ || !controller_) return;

    auto target = hitTest_->HitTest(x, y);
    if (target.type == LayerPanelHitType::LayerRow) {
        controller_->BeginRename(target.layerId);
    }
}
