#include "InkEngine.h"
#include "InkStrokeBuffer.h"
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
    inkBuffer_.Begin(currentStroke_);
    return true;
}

bool InkEngine::Move(POINT viewPoint, float pressure) {
    if (!drawing_ || !document_) return false;
    if (tool_ == MosuanTool::Lasso) { lasso_.push_back(viewPoint); return true; }
    if (tool_ == MosuanTool::Eraser) return EraseAt(viewPoint, 16.0f);
    if (tool_ == MosuanTool::Line) { lineEnd_ = viewPoint; return true; }
    InkPoint point = ToInkPoint(viewPoint, pressure);
    StrokeDynamics::SmoothPoint(currentStroke_, point);
    inkBuffer_.Append(point);
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
    inkBuffer_.Clear();
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
    selectedStrokes_.clear();
    lasso_.clear();
    return true;
}

void InkEngine::Cancel() {
    drawing_ = false;
    currentStroke_ = {};
    inkBuffer_.Clear();
    lasso_.clear();
}

void InkEngine::Draw(HDC hdc) const {
    if (document_) {
        const auto& strokes = document_->LayerStrokes(document_->CurrentPage(), activeLayerId_);
        for (const auto& stroke : strokes) StrokeRenderer::Draw(hdc, stroke, transform_, style_);
    }
    if (drawing_ && tool_ == MosuanTool::Pen && inkBuffer_.IsDrawing()) {
        StrokeRenderer::Draw(hdc, inkBuffer_.Current(), transform_, style_);
    }
    if (drawing_ && tool_ == MosuanTool::Line) StrokeRenderer::DrawLine(hdc, lineStart_, lineEnd_, style_);
}

bool InkEngine::IsSelected(size_t index) const {
    return std::find(selectedStrokes_.begin(), selectedStrokes_.end(), index) != selectedStrokes_.end();
}
