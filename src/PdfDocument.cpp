#include "PdfDocument.h"
#include <windows.h>
#include <algorithm>
#include <cstddef>

namespace {
std::string Utf8(const std::wstring& value) {
    if (value.empty()) return {};
    const int size = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), result.data(), size, nullptr, nullptr);
    return result;
}

void Downsample2x(const std::vector<std::uint8_t>& src, int srcW, int srcH,
                  std::vector<std::uint8_t>& dst, int dstW, int dstH) {
    dst.resize(static_cast<size_t>(dstW) * static_cast<size_t>(dstH) * 4u);
    for (int y = 0; y < dstH; ++y) {
        const int sy0 = y * 2;
        const int sy1 = (std::min)(sy0 + 1, srcH - 1);
        for (int x = 0; x < dstW; ++x) {
            const int sx0 = x * 2;
            const int sx1 = (std::min)(sx0 + 1, srcW - 1);
            const size_t p00 = (static_cast<size_t>(sy0) * srcW + sx0) * 4u;
            const size_t p10 = (static_cast<size_t>(sy0) * srcW + sx1) * 4u;
            const size_t p01 = (static_cast<size_t>(sy1) * srcW + sx0) * 4u;
            const size_t p11 = (static_cast<size_t>(sy1) * srcW + sx1) * 4u;
            const size_t d = (static_cast<size_t>(y) * dstW + x) * 4u;
            for (int c = 0; c < 4; ++c) {
                dst[d + c] = static_cast<std::uint8_t>((
                    static_cast<unsigned>(src[p00 + c]) +
                    static_cast<unsigned>(src[p10 + c]) +
                    static_cast<unsigned>(src[p01 + c]) +
                    static_cast<unsigned>(src[p11 + c]) + 2u) / 4u);
            }
        }
    }
}
}

PdfDocument::PdfDocument() { FPDF_InitLibrary(); }
PdfDocument::~PdfDocument() { Close(); FPDF_DestroyLibrary(); }

bool PdfDocument::Open(const std::wstring& path) {
    Close();
    const std::string utf8 = Utf8(path);
    document_ = FPDF_LoadDocument(utf8.c_str(), nullptr);
    return document_ != nullptr;
}

void PdfDocument::Close() {
    if (page_) { FPDF_ClosePage(page_); page_ = nullptr; }
    if (document_) { FPDF_CloseDocument(document_); document_ = nullptr; }
}

int PdfDocument::PageCount() const { return document_ ? FPDF_GetPageCount(document_) : 0; }

bool PdfDocument::PageSize(int index, float& width, float& height) const {
    if (!document_ || index < 0 || index >= PageCount()) return false;
    if (page_) FPDF_ClosePage(page_);
    page_ = FPDF_LoadPage(document_, index);
    if (!page_) return false;
    width = FPDF_GetPageWidthF(page_);
    height = FPDF_GetPageHeightF(page_);
    return width > 0 && height > 0;
}

bool PdfDocument::RenderPage(int index, int width, int height, std::vector<std::uint8_t>& pixels) const {
    if (!document_ || index < 0 || index >= PageCount() || width <= 0 || height <= 0) return false;
    if (page_) FPDF_ClosePage(page_);
    page_ = FPDF_LoadPage(document_, index);
    if (!page_) return false;

    // Render at 2x and downsample. This is intentional supersampling: it makes
    // thin Chinese strokes, math symbols, fraction bars and diagonal lines
    // much cleaner than rendering directly at the final display resolution.
    const int renderW = width * 2;
    const int renderH = height * 2;
    std::vector<std::uint8_t> hiRes(static_cast<size_t>(renderW) * static_cast<size_t>(renderH) * 4u);

    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(renderW, renderH, FPDFBitmap_BGRA, hiRes.data(), renderW * 4);
    if (!bitmap) return false;
    FPDFBitmap_FillRect(bitmap, 0, 0, renderW, renderH, 0xFFFFFFFF);

    // Keep PDFium's normal anti-aliasing and explicitly request high-quality
    // image/path halftoning. Do NOT use NO_SMOOTHTEXT / NO_SMOOTHIMAGE.
    const int flags = FPDF_ANNOT | FPDF_RENDER_FORCEHALFTONE;
    FPDF_RenderPageBitmap(bitmap, page_, 0, 0, renderW, renderH, 0, flags);
    FPDFBitmap_Destroy(bitmap);

    Downsample2x(hiRes, renderW, renderH, pixels, width, height);
    return true;
}
