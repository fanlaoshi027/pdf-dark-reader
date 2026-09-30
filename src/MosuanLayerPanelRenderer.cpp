#include "MosuanLayerPanelRenderer.h"
#include "MosuanLayerPanelLayout.h"
#include "MosuanLayerThumbnailRenderer.h"

#include <windows.h>

void MosuanLayerPanelRenderer::PaintBackground(HDC hdc, const RECT& panel)
{
    HBRUSH bg = CreateSolidBrush(RGB(25, 27, 32));
    FillRect(hdc, &panel, bg);
    DeleteObject(bg);
}

void MosuanLayerPanelRenderer::PaintTitle(HDC hdc, const RECT& rect)
{
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(220, 225, 235));
    DrawTextW(hdc, L"图层", -1, const_cast<RECT*>(&rect), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void MosuanLayerPanelRenderer::PaintLayerRow(HDC hdc, const RECT& row, const LayerItem& layer, bool active)
{
    HBRUSH brush = CreateSolidBrush(active ? RGB(45, 50, 60) : RGB(30, 33, 39));
    FillRect(hdc, &row, brush);
    DeleteObject(brush);

    RECT text = row;
    text.left += 54;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(210, 215, 225));
    DrawTextW(hdc, layer.name, -1, &text, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}
