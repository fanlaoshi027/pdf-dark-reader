#pragma once

namespace InkRefreshPolicy {

// Keeps pen rendering independent from expensive PDF redraw operations.
// The input layer can request lightweight ink-only refreshes while the
// document layer keeps its cached bitmap.
struct State {
    bool inkDirty = false;
    bool pdfDirty = false;
    void MarkInk() { inkDirty = true; }
    void MarkPdf() { pdfDirty = true; }
    void ClearInk() { inkDirty = false; }
    void ClearPdf() { pdfDirty = false; }
};

}
