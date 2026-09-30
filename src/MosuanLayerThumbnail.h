#pragma once

#include <windows.h>
#include "LayerSystem.h"

class MosuanLayerThumbnail {
public:
    static void Paint(HDC hdc, const RECT& rect, const LayerItem& layer, bool active);
};
