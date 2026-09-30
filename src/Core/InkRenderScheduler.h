#pragma once

#include <cstdint>

// Controls incremental ink redraw scheduling.
// Keeps pen rendering responsive by separating input frequency
// from expensive PDF/window refresh operations.
class InkRenderScheduler {
public:
    void MarkInkDirty() {
        dirty_ = true;
        ++generation_;
    }

    void MarkPdfDirty() {
        pdfDirty_ = true;
    }

    bool ConsumeInkDirty() {
        const bool value = dirty_;
        dirty_ = false;
        return value;
    }

    bool ConsumePdfDirty() {
        const bool value = pdfDirty_;
        pdfDirty_ = false;
        return value;
    }

    uint64_t Generation() const {
        return generation_;
    }

private:
    bool dirty_ = false;
    bool pdfDirty_ = false;
    uint64_t generation_ = 0;
};
