#pragma once

#include <windows.h>
#include <cstddef>

struct MosuanLayerRowLayout {
    RECT bounds{};
    RECT thumbnail{};
    RECT visibility{};
    RECT lock{};
    RECT remove{};
    int layerId = 0;
};

class MosuanLayerPanelLayout {
public:
    static constexpr int kPanelWidth = 280;
    static constexpr int kRowHeight = 72;
    static MosuanLayerRowLayout MakeRow(const RECT& panel, int index, int layerId);
};
