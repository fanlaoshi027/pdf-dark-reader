#include "InkDocument.h"
#include <algorithm>

void InkDocument::Clear() {
    pages_.clear();
    currentPage_ = 0;
}

InkDocument::PageInk& InkDocument::Page(int pageIndex) {
    auto it = std::find_if(pages_.begin(), pages_.end(), [pageIndex](const PageData& p) {
        return p.pageIndex == pageIndex;
    });
    if (it == pages_.end()) {
        PageData page;
        page.pageIndex = pageIndex;
        pages_.push_back(std::move(page));
        it = std::prev(pages_.end());
    }
    static PageInk result;
    result.pageIndex = pageIndex;
    result.strokes.clear();
    for (const auto& layer : it->layers) {
        result.strokes.insert(result.strokes.end(), layer.strokes.begin(), layer.strokes.end());
    }
    return result;
}

const InkDocument::PageInk* InkDocument::FindPage(int pageIndex) const {
    static PageInk result;
    auto it = std::find_if(pages_.begin(), pages_.end(), [pageIndex](const PageData& p) {
        return p.pageIndex == pageIndex;
    });
    if (it == pages_.end()) return nullptr;
    result.pageIndex = pageIndex;
    result.strokes.clear();
    for (const auto& layer : it->layers) {
        result.strokes.insert(result.strokes.end(), layer.strokes.begin(), layer.strokes.end());
    }
    return &result;
}

void InkDocument::SetCurrentPage(int pageIndex) { currentPage_ = pageIndex; }

void InkDocument::AddStroke(int layerId, InkStroke stroke) {
    PageData* page = nullptr;
    auto it = std::find_if(pages_.begin(), pages_.end(), [this](const PageData& p) {
        return p.pageIndex == currentPage_;
    });
    if (it == pages_.end()) {
        PageData data;
        data.pageIndex = currentPage_;
        pages_.push_back(std::move(data));
        page = &pages_.back();
    } else page = &*it;

    auto layer = std::find_if(page->layers.begin(), page->layers.end(), [layerId](const LayerInk& l) {
        return l.layerId == layerId;
    });
    if (layer == page->layers.end()) {
        page->layers.push_back(LayerInk{layerId, {}});
        layer = std::prev(page->layers.end());
    }
    layer->strokes.push_back(std::move(stroke));
}

const std::vector<InkStroke>& InkDocument::LayerStrokes(int pageIndex, int layerId) const {
    static const std::vector<InkStroke> empty;
    auto page = std::find_if(pages_.begin(), pages_.end(), [pageIndex](const PageData& p) {
        return p.pageIndex == pageIndex;
    });
    if (page == pages_.end()) return empty;
    auto layer = std::find_if(page->layers.begin(), page->layers.end(), [layerId](const LayerInk& l) {
        return l.layerId == layerId;
    });
    return layer == page->layers.end() ? empty : layer->strokes;
}

void InkDocument::ClearLayer(int pageIndex, int layerId) {
    auto page = std::find_if(pages_.begin(), pages_.end(), [pageIndex](const PageData& p) {
        return p.pageIndex == pageIndex;
    });
    if (page == pages_.end()) return;
    auto layer = std::find_if(page->layers.begin(), page->layers.end(), [layerId](const LayerInk& l) {
        return l.layerId == layerId;
    });
    if (layer != page->layers.end()) layer->strokes.clear();
}
