#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <windows.h>
#include "fpdfview.h"

class PdfDocument {
public:
    PdfDocument();
    ~PdfDocument();
    bool Open(const std::wstring& path);
    void Close();
    bool IsOpen() const { return document_ != nullptr; }
    int PageCount() const;
    bool PageSize(int index, float& width, float& height) const;
    bool RenderPage(int index, int width, int height, std::vector<std::uint8_t>& pixels) const;
private:
    FPDF_DOCUMENT document_ = nullptr;
    mutable FPDF_PAGE page_ = nullptr;
};
