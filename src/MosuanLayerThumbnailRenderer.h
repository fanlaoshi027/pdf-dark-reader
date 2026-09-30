#pragma once

#include <windows.h>
#include "LayerSystem.h"

class MosuanLayerThumbnailRenderer {
public:
    static void RenderLayerPreview(HDC hdc,
                                   const RECT& rect,
                                   const LayerItem& layer,
                                   bool selected);
};
