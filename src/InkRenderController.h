#pragma once

#include "InkPipeline.h"
#include "InkVectorGeometry.h"
#include "InkVectorRenderer.h"

class InkRenderController {
public:
    explicit InkRenderController(InkVectorRenderer& renderer) : renderer_(renderer) {}

    void Render(const std::vector<InkSample>& samples,
                std::uint32_t color,
                float baseWidth,
                bool dashed);

private:
    InkVectorRenderer& renderer_;
    InkPipeline pipeline_;
};
