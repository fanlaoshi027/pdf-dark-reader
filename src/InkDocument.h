#pragma once
#include <cstddef>
#include <vector>
#include "LayerSystem.h"

struct PageInk {
    int pageIndex = 0;
    std::vector<InkStroke> strokes;
};

class InkDocument {
public:
    void Clear();
    PageInk& Page(int pageIndex);
    const PageInk* FindPage(int pageIndex) const;
    void SetCurrentPage(int pageIndex);
    int CurrentPage() const { return currentPage_; }
    void AddStroke(int layerId, InkStroke stroke);
    const std::vector<InkStroke>& LayerStrokes(int pageIndex, int layerId) const;
    void ClearLayer(int pageIndex, int layerId);

private:
    struct LayerInk {
        int layerId = 0;
        std::vector<InkStroke> strokes;
    };
    struct PageData {
        int pageIndex = 0;
        std::vector<LayerInk> layers;
    };
    std::vector<PageData> pages_;
    int currentPage_ = 0;
};
