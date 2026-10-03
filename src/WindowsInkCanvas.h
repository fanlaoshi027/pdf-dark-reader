#pragma once

#include <windows.h>
#include "WindowsInkHost.h"

// Independent handwriting surface. It deliberately knows nothing about PDFium
// or the PDF document model. The owner supplies only its screen-space bounds.
class WindowsInkCanvas {
public:
    WindowsInkCanvas() = default;
    ~WindowsInkCanvas();

    WindowsInkCanvas(const WindowsInkCanvas&) = delete;
    WindowsInkCanvas& operator=(const WindowsInkCanvas&) = delete;

    bool Create(HWND parent);
    void Resize(const RECT& bounds);
    void SetVisible(bool visible);
    bool IsReady() const { return ready_; }
    void Clear();
    void Shutdown();

private:
    static LRESULT CALLBACK CanvasProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    HWND parent_ = nullptr;
    HWND hwnd_ = nullptr;
    WindowsInkHost ink_;
    bool ready_ = false;
};
