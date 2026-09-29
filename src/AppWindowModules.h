#pragma once

// AppWindow is intentionally split by responsibility.  The concrete module
// implementations are kept in separate translation units as the project grows.
//
// PDF lifecycle      -> AppWindowPdf.*
// input/commands      -> AppWindowInput.*
// toolbar             -> AppWindowToolbar.*
// painting/layout     -> AppWindowPaint.*
//
// This header contains only the small shared surface needed by those modules.
namespace mosuan {
struct ViewportState {
    int scrollY = 0;
    double zoom = 1.0;
    bool fitWidth = false;
};
}
