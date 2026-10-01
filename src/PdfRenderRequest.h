#pragma once

#include <cstdint>
#include <vector>

struct PdfRenderRequest {
    int pageIndex = 0;
    int pixelWidth = 0;
    int pixelHeight = 0;
};

struct PdfRenderResult {
    bool success = false;
    std::vector<std::uint8_t> pixels;
};
