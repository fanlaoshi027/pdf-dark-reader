#include "InkPipeline.h"

#include "InkPressure.h"
#include "InkSmoother.h"
#include "InkTaper.h"
#include "InkStrokeBuilder.h"

VectorStroke InkPipeline::BuildStroke(const std::vector<InkSample>& samples,
                                      std::uint32_t color,
                                      float baseWidth,
                                      bool dashed) const {
    if (samples.empty()) return {};

    std::vector<InkSample> processed = samples;
    for (auto& sample : processed) {
        sample.pressure = InkPressure::Apply(sample.pressure);
    }

    processed = InkSmoother::Smooth(processed);
    processed = InkTaper::Apply(processed);
    return InkStrokeBuilder::Build(processed, color, baseWidth, dashed);
}
