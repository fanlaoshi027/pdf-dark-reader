#pragma once

#include <unordered_map>
#include <windows.h>

class MosuanLayerThumbnailCache {
public:
    void Clear();
    void Remove(int layerId);
    void MarkDirty(int layerId);
    bool IsDirty(int layerId) const;

private:
    std::unordered_map<int, bool> dirty_;
};
