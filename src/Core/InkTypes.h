#pragma once

#include <cstdint>
#include <vector>

// Platform-neutral ink and document types.
// Native Windows/macOS input adapters convert pointer events into these types.
struct PdfViewTransform {
    double scale = 1.0;
    double originX = 0.0;
    double originY = 0.0;
    int pageWidth = 0;
    int pageHeight = 0;
    int scrollY = 0;
};

struct InkPoint {
    double pdfX = 0.0;
    double pdfY = 0.0;
    float pressure = 0.5f;
};

struct InkStroke {
    std::vector<InkPoint> points;
};

enum class MosuanTool : std::uint8_t {
    Pen,
    Ruler,
    Lasso,
    Eraser,
    Line
};

enum class LayerKind : std::uint8_t {
    Background,
    Pdf,
    Ink
};
