#include "InkEngine.h"
#include "LayerTransform.h"
#include <algorithm>
#include <cmath>

InkPoint InkEngine::ToInkPoint(POINT viewPoint, float pressure) const {
    POINT p = LayerTransform::ViewToPdf(transform_, viewPoint.x, viewPoint.y);
    InkPoint result;
    result.pdfX = static_cast<double>(p.x);
    result.pdfY = static_cast<double>(p.y);
    result.pressure = StrokeDynamics::ClampPressure(pressure);
    return result;
}

bool InkEngine::Begin(POINT viewPoint, float pressure) {
    if (!document_) return false;
    if (tool_ == MosuanTool::Eraser || tool_ == MosuanTool::Lasso) {
        drawing_ = true;
        if (tool_ == MosuanTool::Lasso) lasso_.clear();
        if (tool_ == MosuanTool::Lasso) lasso_.push_back(viewPoint);
        return true;
    }
    drawing_ = true;
    currentStroke_.points.clear();
    currentStroke_.points.push_back(ToInkPoint(viewPoint, pressure));
    lineStart_ = lineEnd_ = viewPoint;
    return true;
}

bool InkEngine::Move(POINT viewPoint, float pressure) {
    if (!drawing_ || !document_) return false;
    if (tool_ == MosuanTool::Lasso) {
        lasso_.push_back(viewPoint);
        return true;
    }
    if (tool_ == MosuanTool::Eraser) {
        return EraseAt(viewPoint, 16.0f);
    }
    lineEnd_ = viewPoint;
    if (tool_ == MosuanTool::Line) return true;
    StrokeDynamics::SmoothPoint(currentStroke_, ToInkPoint(viewPoint, pressure));
    return true;
}

bool InkEngine::End(POINT viewPoint, float pressure) {
    if (!drawing_ || !document_) return false;
    if (tool_ == MosuanTool::Lasso) {
        lasso_.push_back(viewPoint);
        drawing_ = false;
        return FinishLasso();
    }
    if (tool_ == MosuanTool::Eraser) {
        drawing_ = false;
        return true;
    }
    lineEnd_ = viewPoint;
    if (tool_ == MosuanTool::Line) {
        InkStroke line;
        line.points.push_back(ToInkPoint(lineStart_, pressure));
        line.points.push_back(ToInkPoint(lineEnd_, pressure));
        document_->AddStroke(activeLayerId_, std::move(line));
    } else {
        StrokeDynamics::SmoothPoint(currentStroke_, ToInkPoint(viewPoint, pressure));
        StrokeDynamics::ApplyTaper(currentStroke_);
        document_->AddStroke(activeLayerId_, std::move(currentStroke_));
        currentStroke_.points.clear();
    }
    drawing_ = false;
    return true;
}

bool InkEngine::EraseAt(POINT viewPoint, float radius) {
    if (!document_) return false;
    const auto& strokes = document_->LayerStrokes(document_->CurrentPage(), activeLayerId_);
    std::vector<InkStroke> keep;
    const double r2 = static_cast<double>(radius * radius);
    for (const auto& stroke : strokes) {
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
    lasso_.clear();
    return true;
}

void InkEngine::Cancel() {
    drawing_ = false;
    currentStroke_.points.clear();
    lasso_.clear();
}

void InkEngine::Draw(HDC hdc) const {
    if (document_) {
        const auto& strokes = document_->LayerStrokes(document_->CurrentPage(), activeLayerId_);
        for (const auto& stroke : strokes) StrokeRenderer::Draw(hdc, stroke, transform_, style_);
    }
    if (drawing_ && tool_ == MosuanTool::Pen && !currentStroke_.points.empty()) {
        StrokeRenderer::Draw(hdc, currentStroke_, transform_, style_);
    }
    if (drawing_ && tool_ == MosuanTool::Line) {
        StrokeRenderer::DrawLine(hdc, lineStart_, lineEnd_, style_);
    }
}
