#include "AppWindowPdf.h"
#include "AppWindow.h"
#include "PdfThumbnailRail.h"
#include <algorithm>
#include <cmath>
#include <commdlg.h>

namespace {
constexpr int kToolbarHeight = 58;

void InvertBgra(std::vector<std::uint8_t>& pixels, const InvertSettings& s) {
    if (!s.enabled) return;
    const float amount = std::clamp(s.strength, 0.0f, 1.0f);
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        const float b = pixels[i] / 255.0f, g = pixels[i + 1] / 255.0f, r = pixels[i + 2] / 255.0f;
        const float ir = r + (1.0f - 2.0f * r) * amount;
        const float ig = g + (1.0f - 2.0f * g) * amount;
        const float ib = b + (1.0f - 2.0f * b) * amount;
        const float orr = -0.574f * ir + 1.430f * ig + 0.144f * ib;
        const float org =  0.426f * ir + 0.430f * ig + 0.144f * ib;
        const float orb =  0.426f * ir + 1.430f * ig - 0.856f * ib;
        pixels[i] = static_cast<std::uint8_t>(std::lround(std::clamp(orb, 0.0f, 1.0f) * 255.0f));
        pixels[i + 1] = static_cast<std::uint8_t>(std::lround(std::clamp(org, 0.0f, 1.0f) * 255.0f));
        pixels[i + 2] = static_cast<std::uint8_t>(std::lround(std::clamp(orr, 0.0f, 1.0f) * 255.0f));
    }
}

void UpdateTransform(AppWindow& app) {
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    RECT viewport{PdfThumbnailRail::kWidth, kToolbarHeight, rc.right, rc.bottom};
    app.layers().Resize(viewport);
    float pageW = 1, pageH = 1;
    if (!app.pdf().PageSize(app.pageIndex(), pageW, pageH)) return;
    const int availableW = (std::max)(1, static_cast<int>(rc.right) - PdfThumbnailRail::kWidth - 20);
    const int x = PdfThumbnailRail::kWidth + (std::max)(10, (availableW - app.renderWidth()) / 2);
    const int y = kToolbarHeight + 10 - app.scrollY();
    const double scale = pageW > 0.0 ? static_cast<double>(app.renderWidth()) / pageW : 1.0;
    PdfViewTransform t{};
    t.scale = scale;
    t.originX = x;
    t.originY = y - kToolbarHeight;
    t.pageWidth = static_cast<int>(pageW);
    t.pageHeight = static_cast<int>(pageH);
    app.layers().SetTransform(t);
}
}

void AppWindowPdf::Open(AppWindow& app) {
    OPENFILENAMEW ofn{};
    wchar_t file[MAX_PATH]{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = app.hwnd();
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"PDF files (*.pdf)\0*.pdf\0All files (*.*)\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;

    if (!app.pdf().Open(file)) {
        MessageBoxW(app.hwnd(), L"无法打开这个 PDF 文件。", L"PDF Dark Reader", MB_ICONERROR);
        return;
    }

    PdfThumbnailRail::ResetCache();
    app.SetPageIndex(0);
    app.SetInvertState(false);
    app.SetZoom(1.0);
    app.SetFitWidth(false);
    app.SetScrollY(0);
    app.layers().ClearInk();
    app.SetActiveTool(MosuanTool::Pen);
    app.layers().SetTool(MosuanTool::Pen);
    FitWidth(app);
}

void AppWindowPdf::Render(AppWindow& app) {
    if (!app.pdf().IsOpen()) return;
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    const int availableW = (std::max)(200, static_cast<int>(rc.right) - PdfThumbnailRail::kWidth - 40);
    const int availableH = (std::max)(200, static_cast<int>(rc.bottom) - kToolbarHeight - 20);
    float pageW = 1, pageH = 1;
    if (!app.pdf().PageSize(app.pageIndex(), pageW, pageH)) return;
    const double fitScale = std::min(static_cast<double>(availableW) / pageW, static_cast<double>(availableH) / pageH);
    const double scale = app.fitWidth() ? static_cast<double>(availableW) / pageW : fitScale * app.zoom();
    const int width = (std::max)(1, static_cast<int>(pageW * scale));
    const int height = (std::max)(1, static_cast<int>(pageH * scale));
    std::vector<std::uint8_t> pixels;
    if (!app.pdf().RenderPage(app.pageIndex(), width, height, pixels)) return;
    InvertSettings& settings = app.invertSettings();
    settings.enabled = app.invertEnabled();
    InvertBgra(pixels, settings);
    app.SetRenderSize(width, height);
    app.SetPixels(std::move(pixels));
    const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight);
    const int maxScroll = (std::max)(0, height - viewportH + 20);
    app.SetScrollY(std::clamp(app.scrollY(), 0, maxScroll));
    app.UpdateScrollBar();
    app.UpdateToolbarText();
    UpdateTransform(app);
    app.Refresh();
}

void AppWindowPdf::Zoom(AppWindow& app, double factor) {
    if (!app.pdf().IsOpen()) return;
    app.SetFitWidth(false);
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight);
    const int oldHeight = (std::max)(1, app.renderHeight());
    const double centerRatio = std::clamp((app.scrollY() + viewportH * 0.5) / static_cast<double>(oldHeight), 0.0, 1.0);
    app.SetZoom(std::clamp(app.zoom() * factor, 0.5, 4.0));
    Render(app);
    const int newMax = (std::max)(0, app.renderHeight() - viewportH + 20);
    app.SetScrollY(std::clamp(static_cast<int>(centerRatio * app.renderHeight() - viewportH * 0.5), 0, newMax));
    app.UpdateScrollBar();
    UpdateTransform(app);
    app.Refresh();
}

void AppWindowPdf::FitWidth(AppWindow& app) {
    if (!app.pdf().IsOpen()) return;
    app.SetFitWidth(true);
    app.SetScrollY(0);
    Render(app);
}

void AppWindowPdf::GoPage(AppWindow& app, int delta) {
    if (!app.pdf().IsOpen()) return;
    const int next = app.pageIndex() + delta;
    if (next < 0 || next >= app.pdf().PageCount()) return;
    app.SetPageIndex(next);
    app.SetScrollY(0);
    app.layers().ClearInk();
    Render(app);
}

void AppWindowPdf::Scroll(AppWindow& app, int delta) {
    if (!app.pdf().IsOpen()) return;
    RECT rc{}; GetClientRect(app.hwnd(), &rc);
    const int viewportH = (std::max)(1, static_cast<int>(rc.bottom) - kToolbarHeight);
    const int maxScroll = (std::max)(0, app.renderHeight() - viewportH + 20);
    if (maxScroll > 0) {
        const int old = app.scrollY();
        app.SetScrollY(std::clamp(old + delta, 0, maxScroll));
        if (old != app.scrollY()) {
            app.UpdateScrollBar();
            UpdateTransform(app);
            app.Refresh();
        }
        return;
    }
    if (delta > 0) GoPage(app, -1);
    else if (delta < 0) GoPage(app, 1);
}
