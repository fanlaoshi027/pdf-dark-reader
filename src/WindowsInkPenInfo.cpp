#include "WindowsInkPenInfo.h"

bool WindowsInkPenInfoReader::Read(UINT32 pointerId, WindowsInkPenInfo& out) const {
    POINTER_PEN_INFO info{};
    if (!GetPointerPenInfo(pointerId, &info)) return false;
    out.pointerId = pointerId;
    out.pressure = info.pressure;
    out.rotation = info.rotation;
    out.tiltX = info.tiltX;
    out.tiltY = info.tiltY;
    out.pixelPosition = info.pointerInfo.ptPixelLocation;
    return true;
}
