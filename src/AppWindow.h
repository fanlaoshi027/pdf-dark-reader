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

    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    PdfDocument pdf_;
    std::vector<std::uint8_t> pixels_;
    int pageIndex_ = 0;
    int renderWidth_ = 0;
    int renderHeight_ = 0;
    bool invert_ = false;
    InvertSettings invertSettings_;
};
