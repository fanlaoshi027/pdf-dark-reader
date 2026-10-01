#pragma once
#include "VectorStroke.h"
#include <windows.h>

class InkStrokeRasterizer {
public:
    void Draw(HDC dc, const VectorStroke& stroke) const;
};
