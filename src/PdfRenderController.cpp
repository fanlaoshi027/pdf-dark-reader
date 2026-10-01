#include "PdfRenderController.h"
#include <algorithm>
#include <cstddef>

namespace {
void ApplyInvert(std::vector<std::uint8_t>& pixels, double strength) {
    strength = (std::max)(0.0, (std::min)(1.0, strength));
    if (strength <= 0.0) return;
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        for (int c = 0; c < 3; ++c) {
            const double source = pixels[i + c];
            const double inverted = 255.0 - source;
            pixels[i + c] = static_cast<std::uint8_t>(source + (inverted - source) * strength + 0.5);
        }
    }
}
}

std::size_t PdfRenderController::CacheKeyHash::operator()(const CacheKey& key) const noexcept {
    std::size_t h = static_cast<std::size_t>(key.page + 1);
    h = h * 31u + static_cast<std::size_t>(key.width);
    h = h * 31u + static_cast<std::size_t>(key.height);
    h = h * 31u + static_cast<std::size_t>(key.invert);
    h = h * 31u + static_cast<std::size_t>(key.invertStrength);
    return h;
}

void PdfRenderController::ClearCache() {
    cache_.clear();
}

void PdfRenderController::RemoveCachedPage(int pageIndex) {
    for (auto it = cache_.begin(); it != cache_.end();) {
        if (it->first.page == pageIndex) it = cache_.erase(it);
        else ++it;
    }
}

bool PdfRenderController::RenderPage(int pageIndex, int pixelWidth, int pixelHeight,
                                     std::vector<std::uint8_t>& pixels) {
    if (pageIndex < 0 || pixelWidth <= 0 || pixelHeight <= 0) return false;

    CacheKey key{pageIndex, pixelWidth, pixelHeight, settings_.invert,
                 static_cast<int>(settings_.invertStrength * 100.0 + 0.5)};
    const auto found = cache_.find(key);
    if (found != cache_.end()) {
        pixels = found->second;
        return true;
    }

    if (!document_.RenderPage(pageIndex, pixelWidth, pixelHeight, pixels)) return false;
    if (settings_.invert) ApplyInvert(pixels, settings_.invertStrength);

    cache_.emplace(key, pixels);
    return true;
}

void PdfRenderController::SetSettings(const PdfRenderSettings& settings) {
    const bool renderChanged = settings_.supersample != settings.supersample ||
                               settings_.invert != settings.invert ||
                               settings_.invertStrength != settings.invertStrength;
    settings_ = settings;
    if (renderChanged) cache_.clear();
}
