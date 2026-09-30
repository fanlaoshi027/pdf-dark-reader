#pragma once

#include <windows.h>

class LayerSystem;

class MosuanLayerPanelController {
public:
    void Attach(LayerSystem* system);
    bool HandleClick(POINT point);
    void Refresh();

private:
    LayerSystem* layerSystem_ = nullptr;
};
