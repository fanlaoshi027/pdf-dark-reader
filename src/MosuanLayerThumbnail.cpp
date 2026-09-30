#include "MosuanLayerThumbnail.h"
#include <windows.h>

void MosuanLayerThumbnail::Paint(HDC hdc, const RECT& rect, const LayerItem& layer, bool active) {
    HBRUSH bg = CreateSolidBrush(active ? RGB(55,60,72) : RGB(38,41,48));
    FillRect(hdc, &rect, bg);
    DeleteObject(bg);

    RECT paper{rect.left + 8, rect.top + 8, rect.right - 8, rect.bottom - 8};
    HBRUSH paperBrush = CreateSolidBrush(RGB(245,245,242));
    FillRect(hdc, &paper, paperBrush);
    DeleteObject(paperBrush);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(90,95,105));
    HGDIOBJ old = SelectObject(hdc, pen);

    if (layer.kind == LayerKind::Background) {
        MoveToEx(hdc, paper.left + 8, paper.top + 18, nullptr);
        LineTo(hdc, paper.right - 8, paper.top + 18);
    } else if (layer.kind == LayerKind::Pdf) {
        Rectangle(hdc, paper.left + 12, paper.top + 12, paper.right - 12, paper.bottom - 12);
    } else {
        MoveToEx(hdc, paper.left + 10, paper.top + 20, nullptr);
        LineTo(hdc, paper.right - 10, paper.top + 20);
        MoveToEx(hdc, paper.left + 10, paper.top + 35, nullptr);
        LineTo(hdc, paper.right - 18, paper.top + 35);
    }

    SelectObject(hdc, old);
    DeleteObject(pen);
}
