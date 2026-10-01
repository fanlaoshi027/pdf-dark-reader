#include "InkRenderController.h"

void InkRenderController::Render(const std::vector<InkSample>& samples,
                                 std::uint32_t color,
                                 float baseWidth,
                                 bool dashed) {
    if (samples.empty()) return;

    const auto stroke = pipeline_.BuildStroke(samples, color, baseWidth, dashed);
    std::vector<InkRenderPoint> renderPoints;
    renderPoints.reserve(stroke.points.size());

    for (const auto& point : stroke.points) {
        InkRenderPoint renderPoint;
        renderPoint.x = point.x;
        renderPoint.y = point.y;
        renderPoint.width = baseWidth * std::max(0.05f, point.pressure);
        renderPoints.push_back(renderPoint);
    }

    const auto geometry = InkVectorGeometry::Build(renderPoints);
    renderer_.Render(geometry);
}
