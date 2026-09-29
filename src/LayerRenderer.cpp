#include "LayerRenderer.h"
#include <windowsx.h>

void LayerRenderer::DrawBackground(HDC hdc, const RECT& viewport, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, &viewport, brush);
    DeleteObject(brush);
}

void LayerRenderer::DrawInk(HDC hdc, const LayerSystem& layers) {
    // Rendering is intentionally isolated from input/model code.
    // The existing LayerSystem overlay remains the source of truth until
    // stroke storage is migrated fully into per-layer containers.
    layers.PaintOverlay(hdc);
}
