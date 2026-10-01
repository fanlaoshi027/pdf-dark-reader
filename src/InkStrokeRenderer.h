#pragma once
#include "VectorStroke.h"
#include <windows.h>

class InkStrokeRenderer {
public:
    void Render(HDC dc, const VectorStroke& stroke) const;
};
