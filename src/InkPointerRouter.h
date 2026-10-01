#pragma once
#include "InkInputAdapter.h"
#include "InkStrokeStore.h"
#include <windows.h>

class InkPointerRouter {
public:
    void Begin(UINT32 pointerId, std::uint32_t color, float width, bool dashed);
    void Update(UINT32 pointerId, double x, double y, float pressure, std::uint64_t timestamp);
    void UpdateWithHistory(HWND hwnd, UINT32 pointerId, double x, double y, float pressure, std::uint64_t timestamp);
    bool End(UINT32 pointerId);
    void Cancel();
    bool Active() const { return input_.Active(); }
    const InkStrokeStore& Store() const { return store_; }
private:
    InkInputAdapter input_;
    InkStrokeStore store_;
    UINT32 pointerId_ = 0;
};
