#include "MosuanLayerThumbnailCache.h"

void MosuanLayerThumbnailCache::MarkDirty(int layerId)
{
    dirtyLayers_.insert(layerId);
}

void MosuanLayerThumbnailCache::Clear(int layerId)
{
    thumbnails_.erase(layerId);
    dirtyLayers_.erase(layerId);
}

bool MosuanLayerThumbnailCache::IsDirty(int layerId) const
{
    return dirtyLayers_.find(layerId) != dirtyLayers_.end();
}

void MosuanLayerThumbnailCache::Set(int layerId, HBITMAP bitmap)
{
    thumbnails_[layerId] = bitmap;
    dirtyLayers_.erase(layerId);
}

HBITMAP MosuanLayerThumbnailCache::Get(int layerId) const
{
    auto it = thumbnails_.find(layerId);
    if (it == thumbnails_.end()) return nullptr;
    return it->second;
}
