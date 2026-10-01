#pragma once

struct PdfViewState {
    int pageIndex = 0;
    int pageCount = 0;
    int renderWidth = 0;
    int renderHeight = 0;
    int scrollY = 0;
    double zoom = 1.0;
    bool fitWidth = false;
    bool invert = false;
};
