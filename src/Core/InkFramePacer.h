#pragma once

#include <chrono>

class InkFramePacer {
public:
    void MarkInkDirty() { inkDirty_ = true; }
    void MarkPdfDirty() { pdfDirty_ = true; }

    bool NeedInkRefresh() const { return inkDirty_; }
    bool NeedPdfRefresh() const { return pdfDirty_; }

    void ConsumeInk() { inkDirty_ = false; }
    void ConsumePdf() { pdfDirty_ = false; }

private:
    bool inkDirty_ = false;
    bool pdfDirty_ = false;
};
