#include "PdfPageCache.h"

void PdfPageCache::Clear() {
    for (auto& [page, bitmap] : cache_) {
        if (bitmap) DeleteObject(bitmap);
    }
    cache_.clear();
}

void PdfPageCache::Remove(int pageIndex) {
    auto it = cache_.find(pageIndex);
    if (it == cache_.end()) return;
    if (it->second) DeleteObject(it->second);
    cache_.erase(it);
}

bool PdfPageCache::Contains(int pageIndex) const {
    return cache_.find(pageIndex) != cache_.end();
}

HBITMAP PdfPageCache::Get(int pageIndex) const {
    auto it = cache_.find(pageIndex);
    return it == cache_.end() ? nullptr : it->second;
}

void PdfPageCache::Put(int pageIndex, HBITMAP bitmap) {
    Remove(pageIndex);
    if (bitmap) cache_[pageIndex] = bitmap;
}
