#pragma once

#include <cstddef>

class PdfCachePolicy {
public:
    explicit PdfCachePolicy(std::size_t maxEntries = 7) : maxEntries_(maxEntries) {}

    std::size_t MaxEntries() const { return maxEntries_; }
    void SetMaxEntries(std::size_t value) { maxEntries_ = value == 0 ? 1 : value; }

private:
    std::size_t maxEntries_;
};
