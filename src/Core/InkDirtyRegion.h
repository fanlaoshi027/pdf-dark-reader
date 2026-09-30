#pragma once

#include <windows.h>

struct InkDirtyRegion {
    RECT rect{};
    bool dirty = false;

    void Clear() {
        dirty = false;
        rect = RECT{};
    }

    void Include(const RECT& value) {
        if (!dirty) {
            rect = value;
            dirty = true;
            return;
        }

        UnionRect(&rect, &rect, &value);
    }
};
