#pragma once

#include <windows.h>

namespace MosuanLayerPanelIcons {

struct IconRect {
    RECT rect{};
};

inline void DrawEye(HDC hdc, const RECT& r, bool visible) {
    RECT box = r;
    if (visible) {
        MoveToEx(hdc, box.left, (box.top + box.bottom) / 2, nullptr);
        LineTo(hdc, box.right, (box.top + box.bottom) / 2);
    } else {
        MoveToEx(hdc, box.left, box.top, nullptr);
        LineTo(hdc, box.right, box.bottom);
    }
}

inline void DrawLock(HDC hdc, const RECT& r, bool locked) {
    RECT box = r;
    if (locked) {
        Rectangle(hdc, box.left, box.top, box.right, box.bottom);
    }
}

}
