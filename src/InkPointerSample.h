#pragma once
#include "InkSample.h"
#include <windows.h>

class InkPointerSample {
public:
    static InkSample FromPointerInfo(const POINTER_PEN_INFO& info, POINT clientPoint, std::uint64_t timestamp) {
        InkSample sample;
        sample.x = static_cast<double>(clientPoint.x);
        sample.y = static_cast<double>(clientPoint.y);
        sample.pressure = static_cast<float>(info.pressure) / 1024.0f;
        sample.pressure = (std::max)(0.0f, (std::min)(1.0f, sample.pressure));
        sample.timestamp = timestamp;
        return sample;
    }
};
