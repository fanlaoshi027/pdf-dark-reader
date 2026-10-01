#include "InkStrokeBuilder.h"

VectorStroke InkStrokeBuilder::Build(const std::vector<InkSample>& samples,
                                     std::uint32_t color,
                                     float baseWidth,
                                     bool dashed) {
    VectorStroke stroke;
    stroke.color = color;
    stroke.baseWidth = baseWidth;
    stroke.dashed = dashed;
    stroke.points.reserve(samples.size());

    for (const auto& sample : samples) {
        stroke.points.push_back({
            sample.x,
            sample.y,
            sample.pressure,
            sample.timestamp
        });
    }
    return stroke;
}
