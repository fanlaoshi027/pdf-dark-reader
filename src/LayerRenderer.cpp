#include "LayerRenderer.h"
#include <windowsx.h>

void LayerRenderer::DrawBackground(HDC hdc, const RECT& viewport, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, &viewport, brush);
    DeleteObject(brush);
}

void LayerRenderer::DrawInk(HDC hdc, LayerSystem& layers) {
    layers.PaintOverlay(hdc);
}
