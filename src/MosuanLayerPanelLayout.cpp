#include "MosuanLayerPanelLayout.h"

namespace MosuanLayerPanelLayout {

LayerRow MakeRow(int index, int top) {
    LayerRow row{};
    row.bounds = {0, top, kPanelWidth, top + kRowHeight};
    row.thumbnail = {8, top + 6, 62, top + 46};
    row.name = {70, top + 8, 180, top + 36};
    row.visible = {188, top + 12, 214, top + 38};
    row.lock = {218, top + 12, 244, top + 38};
    row.remove = {248, top + 12, 274, top + 38};
    row.index = index;
    return row;
}

RECT AddButton() {
    return {8, 0, kPanelWidth - 8, 42};
}

}
