#pragma once
#include <vector>
#include "LayerSystem.h"

struct InkPageData {
    int pageIndex = 0;
    struct LayerInk {
        int layerId = 0;
        std::vector<InkStroke> strokes;
    };
    std::vector<LayerInk> layers;
};

class InkDocument {
public:
    void Clear();
    InkPageData& Page(int pageIndex);
    const InkPageData* FindPage(int pageIndex) const;
    void SetCurrentPage(int pageIndex);
    int CurrentPage() const { return currentPage_; }
    void AddStroke(int layerId, InkStroke stroke);
    const std::vector<InkStroke>& LayerStrokes(int pageIndex, int layerId) const;
    void ClearLayer(int pageIndex, int layerId);

private:
    std::vector<InkPageData> pages_;
    int currentPage_ = 0;
};
