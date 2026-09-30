#pragma once

#include <windows.h>

struct InkRefreshRequest {
    RECT region{};
    bool pending = false;

    void Reset() noexcept {
        region = RECT{};
        pending = false;
    }

    void Add(const RECT& rect) noexcept {
        if (!pending) {
            region = rect;
            pending = true;
            return;
        }
        UnionRect(&region, &region, &rect);
    }

    bool HasRequest() const noexcept {
        return pending;
    }
};
