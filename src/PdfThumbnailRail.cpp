#include "PdfThumbnailRail.h"
#include "AppWindow.h"
#include <algorithm>
#include <vector>

namespace {
constexpr int kHeaderHeight = 34;
constexpr int kThumbW = 82;
constexpr int kThumbH = 108;
constexpr int kGap = 10;
constexpr int kLeft = 15;
constexpr int kTopPadding = 10;

void Fill(HDC hdc, const RECT& r, COLORREF color) {
    HBRUSH b = CreateSolidBrush(color);
    FillRect(hdc, &r, b);
    DeleteObject(b);
}
}

void PdfThumbnailRail::Paint(AppWindow& app, HDC hdc, const RECT& client) {
    if (!app.pdf().IsOpen()) return;

    const int width = kWidth;
    RECT rail{0, kTop, width, client.bottom};
    Fill(hdc, rail, RGB(24, 26, 31));

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(190, 194, 202));
    RECT title{0, kTop + 4, width, kTop + kHeaderHeight};
    DrawTextW(hdc, L"PDF", -1, &title, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    const int count = app.pdf().PageCount();
    const int first = 0;
    const int last = count;

    for (int page = first; page < last; ++page) {
        const int y = kTop + kHeaderHeight + kTopPadding + page * (kThumbH + kGap);
        if (y > client.bottom) break;
        if (y + kThumbH < kTop) continue;

        RECT card{kLeft, y, kLeft + kThumbW, y + kThumbH};
        const bool active = page == app.pageIndex();
        Fill(hdc, card, active ? RGB(49, 55, 67) : RGB(33, 36, 42));

        HPEN border = CreatePen(PS_SOLID, active ? 2 : 1,
                                active ? RGB(100, 150, 235) : RGB(62, 66, 74));
        HGDIOBJ oldPen = SelectObject(hdc, border);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, card.left, card.top, card.right, card.bottom);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(border);

        // Render a small real page image using the same PDF renderer as the main view.
        float pw = 1.0f, ph = 1.0f;
        if (!app.pdf().PageSize(page, pw, ph)) continue;
        const double scale = std::min((kThumbW - 8.0) / pw, (kThumbH - 8.0) / ph);
        const int rw = (std::max)(1, static_cast<int>(pw * scale));
        const int rh = (std::max)(1, static_cast<int>(ph * scale));
        std::vector<std::uint8_t> pixels;
        if (!app.pdf().RenderPage(page, rw, rh, pixels)) continue;

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = rw;
        bmi.bmiHeader.biHeight = -rh;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        const int dx = card.left + (kThumbW - rw) / 2;
        const int dy = card.top + 4;
        StretchDIBits(hdc, dx, dy, rw, rh, 0, 0, rw, rh,
                      pixels.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);

        wchar_t number[16]{};
        wsprintfW(number, L"%d", page + 1);
        SetTextColor(hdc, RGB(175, 180, 190));
        RECT label{card.left, card.bottom - 18, card.right, card.bottom};
        DrawTextW(hdc, number, -1, &label, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

bool PdfThumbnailRail::HitTest(AppWindow& app, int x, int y, int& page) {
    if (!app.pdf().IsOpen() || x < 0 || x >= kWidth || y < kTop) return false;
    const int row = y - (kTop + kHeaderHeight + kTopPadding);
    if (row < 0) return false;
    const int step = kThumbH + kGap;
    const int index = row / step;
    const int inside = row % step;
    if (inside >= kThumbH || index < 0 || index >= app.pdf().PageCount()) return false;
    page = index;
    return true;
}
