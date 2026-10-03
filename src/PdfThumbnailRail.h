#pragma once
#include <windows.h>

class PdfThumbnailRail {
public:
    static constexpr int kWidth = 112;
    static constexpr int kTop = 58;

    // Installs the lightweight per-thread hook used to attach the rail to the
    // existing PDFDarkReader main window. Safe to call repeatedly.
    static void Install();
    static void Ensure(HWND parent);
    static void Resize(HWND parent);
    static void Refresh(HWND parent);
    static void ResetCache();
};
