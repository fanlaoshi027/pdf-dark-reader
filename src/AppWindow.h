#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>
#include "PdfDocument.h"
#include "InvertSettings.h"

class AppWindow {
public:
    bool Create(HINSTANCE instance);
    int Run();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    void Paint(HDC hdc);
    void OpenPdf();
    void RenderCurrentPage();
    void ApplyInvert();
    void CreateToolbar();
    void LayoutToolbar(int width);
    void UpdateScrollBar();
    void ChangeZoom(double factor);
    void ScrollBy(int delta);
    void SetInvert(bool enabled);

    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    PdfDocument pdf_;
    std::vector<std::uint8_t> pixels_;
    int pageIndex_ = 0;
    int renderWidth_ = 0;
    int renderHeight_ = 0;
    int scrollY_ = 0;
    int viewportTop_ = 46;
    double zoom_ = 1.0;
    bool invert_ = false;
    InvertSettings invertSettings_;

    HWND openButton_ = nullptr;
    HWND prevButton_ = nullptr;
    HWND nextButton_ = nullptr;
    HWND zoomOutButton_ = nullptr;
    HWND zoomLabel_ = nullptr;
    HWND zoomInButton_ = nullptr;
    HWND fitButton_ = nullptr;
    HWND invertButton_ = nullptr;
};
