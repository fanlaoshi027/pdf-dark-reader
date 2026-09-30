#include "MosuanLayerPanelHitTest.h"

MosuanLayerPanelHitTarget MosuanLayerPanelHitTest::HitTest(POINT p, const MosuanLayerPanelLayout& layout)
{
    for (const auto& row : layout.Rows()) {
        if (PtInRect(&row.bounds, p)) {
            if (PtInRect(&row.visibleButton, p)) return {HitType::ToggleVisible, row.layerId};
            if (PtInRect(&row.lockButton, p)) return {HitType::ToggleLock, row.layerId};
            if (PtInRect(&row.deleteButton, p)) return {HitType::DeleteLayer, row.layerId};
            return {HitType::SelectLayer, row.layerId};
        }
    }
    if (PtInRect(&layout.AddLayerButton(), p)) return {HitType::AddLayer, 0};
    return {HitType::None, 0};
}
