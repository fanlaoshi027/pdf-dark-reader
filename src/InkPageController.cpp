#include "InkPageController.h"
#include "LayerTransform.h"

void InkPageController::SetPage(int pageIndex) {
    if (document_) document_->SetCurrentPage(pageIndex);
    Cancel();
}

void InkPageController::Begin(POINT viewPoint, float pressure) {
    if (!document_ || !layers_ || !layers_->PenEnabled()) return;
    const POINT p = LayerTransform::ViewToPdf(layers_->Transform(), viewPoint.x, viewPoint.y);
    currentStroke_ = {};
    currentStroke_.points.push_back({static_cast<double>(p.x), static_cast<double>(p.y), StrokeDynamics::ClampPressure(pressure)});
    drawing_ = true;
    lineMode_ = false;
}

void InkPageController::Move(POINT viewPoint, float pressure) {
    if (!drawing_ || lineMode_ || !layers_) return;
    const POINT p = LayerTransform::ViewToPdf(layers_->Transform(), viewPoint.x, viewPoint.y);
    InkPoint point{static_cast<double>(p.x), static_cast<double>(p.y), StrokeDynamics::ClampPressure(pressure)};
    StrokeDynamics::SmoothPoint(currentStroke_, point);
}

void InkPageController::End(POINT viewPoint, float pressure) {
    if (!drawing_ || lineMode_ || !document_ || !layers_) return;
    Move(viewPoint, pressure);
    StrokeDynamics::ApplyTaper(currentStroke_);
    if (!currentStroke_.points.empty()) document_->AddStroke(layers_->ActiveLayerId(), std::move(currentStroke_));
    currentStroke_ = {};
    drawing_ = false;
}

void InkPageController::BeginLine(POINT viewPoint) {
    if (!layers_ || !document_) return;
    lineStart_ = LayerTransform::ViewToPdf(layers_->Transform(), viewPoint.x, viewPoint.y);
    drawing_ = true;
    lineMode_ = true;
}

void InkPageController::EndLine(POINT viewPoint) {
    if (!drawing_ || !lineMode_ || !document_ || !layers_) return;
    const POINT p = LayerTransform::ViewToPdf(layers_->Transform(), viewPoint.x, viewPoint.y);
    InkStroke stroke;
    stroke.points.push_back({static_cast<double>(lineStart_.x), static_cast<double>(lineStart_.y), 1.0f});
    stroke.points.push_back({static_cast<double>(p.x), static_cast<double>(p.y), 1.0f});
    document_->AddStroke(layers_->ActiveLayerId(), std::move(stroke));
    drawing_ = false;
    lineMode_ = false;
}

void InkPageController::Cancel() {
    drawing_ = false;
    lineMode_ = false;
    currentStroke_ = {};
}
