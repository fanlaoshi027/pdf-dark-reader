#pragma once

#include "PdfDocument.h"
#include "PdfRenderSettings.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

class PdfRenderController {
public:
    explicit PdfRenderController(PdfDocument& document) : document_(document) {}

    void ClearCache();
    void RemoveCachedPage(int pageIndex);
    bool RenderPage(int pageIndex, int pixelWidth, int pixelHeight,
                    std::vector<std::uint8_t>& pixels);
    bool RenderPage(int pageIndex, int pixelWidth, int pixelHeight) {
        std::vector<std::uint8_t> scratch;
        return RenderPage(pageIndex, pixelWidth, pixelHeight, scratch);
    }
    void SetSettings(const PdfRenderSettings& settings);
    const PdfRenderSettings& Settings() const { return settings_; }

private:
    struct CacheKey {
        int page = -1;
        int width = 0;
        int height = 0;
        bool invert = false;
        int invertStrength = 100;
        bool operator==(const CacheKey& other) const {
            return page == other.page && width == other.width && height == other.height &&
                   invert == other.invert && invertStrength == other.invertStrength;
        }
    };
    struct CacheKeyHash {
        std::size_t operator()(const CacheKey& key) const noexcept;
    };

    PdfDocument& document_;
    PdfRenderSettings settings_{};
    std::unordered_map<CacheKey, std::vector<std::uint8_t>, CacheKeyHash> cache_;
};
