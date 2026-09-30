#pragma once

#include "MosuanLayerPanelController.h"

class MosuanLayerPanelEventRouter {
public:
    void Attach(MosuanLayerPanelController* controller);
    bool OnMouseDown(int x, int y);
    bool OnMouseDoubleClick(int x, int y);
    bool OnMouseMove(int x, int y);
    bool OnMouseUp(int x, int y);

private:
    MosuanLayerPanelController* controller_ = nullptr;
    bool dragging_ = false;
};
