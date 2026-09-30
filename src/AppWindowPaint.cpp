#include "AppWindowPaint.h"
#include "AppWindow.h"
#include "MosuanFavoritesRail.h"
#include "PdfThumbnailRail.h"
#include <algorithm>

namespace { constexpr int kToolbarHeight = 70; }

void AppWindowPaint::Paint(AppWindow& app, HDC hdc) {
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    const COLORREF background = app.invertEnabled()
        ? RGB(app.invertSettings().backgroundR, app.invertSettings().backgroundG, app.invertSettings().backgroundB)
        : RGB(235, 235, 235);
    HBRUSH brush = CreateSolidBrush(background); FillRect(hdc, &rc, brush); DeleteObject(brush);

    if (!app.pdf().IsOpen() || app.pixels().empty()) {
        RECT body{PdfThumbnailRail::kWidth, kToolbarHeight, rc.right - MosuanFavoritesRail::kWidth, rc.bottom};
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(80,80,80));
        DrawTextW(hdc, L"打开 PDF", -1, &body, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        MosuanFavoritesRail::Paint(app, hdc, rc);
        return;
    }

    PdfThumbnailRail::Paint(app, hdc, rc);

    const int contentLeft = PdfThumbnailRail::kWidth;
    const int contentRight = rc.right - MosuanFavoritesRail::kWidth;
    const int availableW = (std::max)(1, contentRight - contentLeft - 20);
    const int x = contentLeft + (std::max)(10, (availableW - app.renderWidth()) / 2);
    const int y = kToolbarHeight + 10 - app.scrollY();

    // PDF layer is painted first. Ink remains an independent vector layer.
    // This prevents zoom/scroll operations from forcing complete ink redraw.
    if (y < rc.bottom && y + app.renderHeight() > kToolbarHeight) {
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = app.renderWidth();
        bmi.bmiHeader.biHeight = -app.renderHeight();
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(hdc, x, y, app.renderWidth(), app.renderHeight(), 0, 0,
                      app.renderWidth(), app.renderHeight(), app.pixels().data(),
                      &bmi, DIB_RGB_COLORS, SRCCOPY);

        app.SyncInkTransform(x, y);

        // Draw vector ink after PDF rendering.
        // Keeps handwriting sharp during zoom and avoids bitmap scaling blur.
        if (app.layers().MosuanVisible()) app.Ink().Draw(hdc);
    }

    MosuanFavoritesRail::Paint(app, hdc, rc);
}

void AppWindowPaint::DrawToolbarButton(AppWindow&, const DRAWITEMSTRUCT*) {
    // The visible toolbar is owned by MosuanUi. PDF rendering stays independent.
}
