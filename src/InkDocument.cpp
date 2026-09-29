#include "InkDocument.h"
#include <algorithm>

void InkDocument::Clear() {
    pages_.clear();
    currentPage_ = 0;
}

InkPageData& InkDocument::Page(int pageIndex) {
    auto it = std::find_if(pages_.begin(), pages_.end(), [pageIndex](const InkPageData& p) { return p.pageIndex == pageIndex; });
    if (it == pages_.end()) {
        InkPageData page;
        page.pageIndex = pageIndex;
        pages_.push_back(std::move(page));
        return pages_.back();
    }
    return *it;
}

const InkPageData* InkDocument::FindPage(int pageIndex) const {
    auto it = std::find_if(pages_.begin(), pages_.end(), [pageIndex](const InkPageData& p) { return p.pageIndex == pageIndex; });
    return it == pages_.end() ? nullptr : &*it;
}

void InkDocument::SetCurrentPage(int pageIndex) {
    currentPage_ = pageIndex;
    Page(pageIndex);
}

void InkDocument::AddStroke(int layerId, InkStroke stroke) {
    auto& page = Page(currentPage_);
    auto it = std::find_if(page.layers.begin(), page.layers.end(), [layerId](const InkPageData::LayerInk& l) { return l.layerId == layerId; });
    if (it == page.layers.end()) {
        page.layers.push_back({layerId, {}});
        it = std::prev(page.layers.end());
    }
    it->strokes.push_back(std::move(stroke));
}

const std::vector<InkStroke>& InkDocument::LayerStrokes(int pageIndex, int layerId) const {
    static const std::vector<InkStroke> empty;
    const auto* page = FindPage(pageIndex);
    if (!page) return empty;
    auto it = std::find_if(page->layers.begin(), page->layers.end(), [layerId](const InkPageData::LayerInk& l) { return l.layerId == layerId; });
    return it == page->layers.end() ? empty : it->strokes;
}

void InkDocument::ClearLayer(int pageIndex, int layerId) {
    auto& page = Page(pageIndex);
    auto it = std::find_if(page.layers.begin(), page.layers.end(), [layerId](const InkPageData::LayerInk& l) { return l.layerId == layerId; });
    if (it != page.layers.end()) it->strokes.clear();
}
