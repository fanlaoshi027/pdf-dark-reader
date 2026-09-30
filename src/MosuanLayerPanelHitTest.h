#pragma once

#include <windows.h>

struct LayerPanelHitResult {
    enum Type {
        None,
        SelectLayer,
        ToggleVisible,
        ToggleLock,
        DeleteLayer,
        AddLayer
    } type = None;
    int layerId = 0;
};

class MosuanLayerPanelHitTest {
public:
    static LayerPanelHitResult HitTest(POINT pt);
};
