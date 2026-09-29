#include "InkEngine.h"
#include "LayerTransform.h"
#include <algorithm>
#include <cmath>

InkPoint InkEngine::ToInkPoint(POINT viewPoint, float pressure) const {
    POINT p = LayerTransform::ViewToPdf(transform_, viewPoint.x, viewPoint.y);
    return {static_cast<double>(p.x), static_cast<double>(p.y), StrokeDynamics::ClampPressure(pressure)};
}

bool InkEngine::Begin(POINT viewPoint, float pressure) {
    if (!document_) return false;
    drawing_ = true;
    if (tool_ == MosuanTool::Lasso) { lasso_.clear(); lasso_.push_back(viewPoint); return true; }
    if (tool_ == MosuanTool::Eraser) return true;
    if (tool_ == MosuanTool::Line) { lineStart_ = lineEnd_ = viewPoint; return true; }
    currentStroke_ = {};
    currentStroke_.points.push_back(ToInkPoint(viewPoint, pressure));
    return true;
}

bool InkEngine::Move(POINT viewPoint, float pressure) {
    if (!drawing_ || !document_) return false;
    if (tool_ == MosuanTool::Lasso) { lasso_.push_back(viewPoint); return true; }
    if (tool_ == MosuanTool::Eraser) return EraseAt(viewPoint, 16.0f);
    if (tool_ == MosuanTool::Line) { lineEnd_ = viewPoint; return true; }
    StrokeDynamics::SmoothPoint(currentStroke_, ToInkPoint(viewPoint, pressure));
    return true;
}

bool InkEngine::End(POINT viewPoint, float pressure) {
    if (!drawing_ || !document_) return false;
    if (tool_ == MosuanTool::Lasso) { lasso_.push_back(viewPoint); drawing_ = false; return FinishLasso(); }
    if (tool_ == MosuanTool::Eraser) { drawing_ = false; return true; }
    if (tool_ == MosuanTool::Line) {
        lineEnd_ = viewPoint;
        InkStroke line;
        line.points.push_back(ToInkPoint(lineStart_, pressure));
        line.points.push_back(ToInkPoint(lineEnd_, pressure));
        document_->AddStroke(activeLayerId_, std::move(line));
    } else {
        StrokeDynamics::SmoothPoint(currentStroke_, ToInkPoint(viewPoint, pressure));
        StrokeDynamics::ApplyTaper(currentStroke_);
        if (!currentStroke_.points.empty()) document_->AddStroke(activeLayerId_, std::move(currentStroke_));
    }
    currentStroke_ = {};
    drawing_ = false;
    return true;
}

bool InkEngine::EraseAt(POINT viewPoint, float radius) {
    if (!document_) return false;
    const auto& source = document_->LayerStrokes(document_->CurrentPage(), activeLayerId_);
    std::vector<InkStroke> keep;
    const double r2 = static_cast<double>(radius * radius);
    for (const auto& stroke : source) {
        bool hit = false;
        for (const auto& p : stroke.points) {
            POINT v = LayerTransform::PdfToView(transform_, p.pdfX, p.pdfY);
            const double dx = v.x - viewPoint.x;
            const double dy = v.y - viewPoint.y;
            if (dx * dx + dy * dy <= r2) { hit = true; break; }
        }
        if (!hit) keep.push_back(stroke);
    }
    document_->ClearLayer(document_->CurrentPage(), activeLayerId_);
    for (auto& stroke : keep) document_->AddStroke(activeLayerId_, std::move(stroke));
    return true;
}

bool InkEngine::AddLassoPoint(POINT viewPoint) {
    if (!drawing_ || tool_ != MosuanTool::Lasso) return false;
    lasso_.push_back(viewPoint);
    return true;
}

bool InkEngine::FinishLasso() {
    // Selection geometry is kept for the next selection/transform stage.
    selectedStrokes_.clear();
    if (lasso_.size() < 3 || !document_) { lasso_.clear(); return false; }
    const auto& strokes = document_->LayerStrokes(document_->CurrentPage(), activeLayerId_);
    for (size_t i = 0; i < strokes.size(); ++i) {
        for (const auto& point : strokes[i].points) {
            POINT v = LayerTransform::PdfToView(transform_, point.pdfX, point.pdfY);
            bool inside = false;
            for (size_t j = 0, k = lasso_.size() - 1; j < lasso_.size(); k = j++) {
                const POINT a = lasso_[j], b = lasso_[k];
                if (((a.y > v.y) != (b.y > v.y)) &&
                    (v.x < (b.x-a.x) * (v.y-a.y) / static_cast<double>(b.y-a.y) + a.x)) inside = !inside;
            }
            if (inside) { selectedStrokes_.push_back(i); break; }
        }
    }
    lasso_.clear();
    return true;
}

void InkEngine::Cancel() { drawing_ = false; currentStroke_ = {}; lasso_.clear(); }

void InkEngine::Draw(HDC hdc) const {
    if (document_) {
        const auto& strokes = document_->LayerStrokes(document_->CurrentPage(), activeLayerId_);
        for (const auto& stroke : strokes) StrokeRenderer::Draw(hdc, stroke, transform_, style_);
    }
    if (drawing_ && tool_ == MosuanTool::Pen && !currentStroke_.points.empty()) StrokeRenderer::Draw(hdc, currentStroke_, transform_, style_);
    if (drawing_ && tool_ == MosuanTool::Line) StrokeRenderer::DrawLine(hdc, lineStart_, lineEnd_, style_);
}

bool InkEngine::IsSelected(size_t index) const {
    return std::find(selectedStrokes_.begin(), selectedStrokes_.end(), index) != selectedStrokes_.end();
}
