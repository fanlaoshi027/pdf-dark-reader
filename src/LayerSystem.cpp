#include "LayerSystem.h"
#include <algorithm>

bool LayerSystem::IsLayerVisible(int id) const { for (const auto& l : layers_) if (l.id == id) return l.visible; return false; }
bool LayerSystem::IsLayerLocked(int id) const { for (const auto& l : layers_) if (l.id == id) return l.locked; return false; }
