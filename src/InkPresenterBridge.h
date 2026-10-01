#pragma once

#include "InkPointerRouter.h"
#include <windows.h>

class InkPresenterBridge {
public:
    void Begin(UINT32 pointerId, std::uint32_t color, float width, bool dashed);
    void Update(UINT32 pointerId, double x, double y, float pressure, std::uint64_t timestamp);
    bool End(UINT32 pointerId);
    void Cancel();
    InkPointerRouter& Router() { return router_; }
private:
    InkPointerRouter router_;
};
