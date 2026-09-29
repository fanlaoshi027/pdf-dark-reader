#pragma once
#include <windows.h>
#include "LayerSystem.h"

class LayerInput {
public:
    static bool BeginPen(LayerSystem& layers, POINT viewPoint, float pressure = 0.5f);
    static bool UpdatePen(LayerSystem& layers, POINT viewPoint, float pressure = 0.5f);
    static bool EndPen(LayerSystem& layers, POINT viewPoint, float pressure = 0.5f);
    static bool Erase(LayerSystem& layers, POINT viewPoint, int radius = 18);
    static bool AddLassoPoint(LayerSystem& layers, POINT viewPoint);
    static bool FinishLasso(LayerSystem& layers);
};
