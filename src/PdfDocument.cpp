#include "PdfDocument.h"
#include <windows.h>

namespace {
std::string Utf8(const std::wstring& value) {
    if (value.empty()) return {};
    const int size = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), result.data(), size, nullptr, nullptr);
    return result;
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
    pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4u);
    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(width, height, FPDFBitmap_BGRA, pixels.data(), width * 4);
    if (!bitmap) return false;
    FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);
    FPDF_RenderPageBitmap(bitmap, page_, 0, 0, width, height, 0, FPDF_ANNOT);
    FPDFBitmap_Destroy(bitmap);
    return true;
}
