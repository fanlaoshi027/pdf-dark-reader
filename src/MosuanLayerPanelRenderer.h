#pragma once
#include <windows.h>

class LayerSystem;
class MosuanLayerPanelLayout;

class MosuanLayerPanelRenderer {
public:
    static void Paint(HDC hdc, const RECT& panel, LayerSystem& layers);
};
