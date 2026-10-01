#pragma once

#include "PdfDocument.h"
#include "PdfPageCache.h"
#include "PdfRenderSettings.h"

class PdfRenderController {
public:
    explicit PdfRenderController(PdfDocument& document) : document_(document) {}

    void ClearCache();
    HBITMAP RenderPage(int pageIndex, int pixelWidth, int pixelHeight);
    void SetSettings(const PdfRenderSettings& settings) { settings_ = settings; }
    const PdfRenderSettings& Settings() const { return settings_; }

private:
    PdfDocument& document_;
    PdfPageCache cache_;
    PdfRenderSettings settings_{};
};
