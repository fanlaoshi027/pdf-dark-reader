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

// Vector ink point. Designed to preserve pen information instead of bitmap output.
struct InkPoint {
    double pdfX = 0.0;
    double pdfY = 0.0;
    float pressure = 0.5f;
    float tiltX = 0.0f;
    float tiltY = 0.0f;
    std::uint64_t timestamp = 0;
};

struct InkStroke {
    std::uint64_t id = 0;
    std::vector<InkPoint> points;
    float width = 2.0f;
    std::uint32_t color = 0xFF000000;
    bool finished = false;
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
