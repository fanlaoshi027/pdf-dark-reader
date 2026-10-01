#pragma once
#include "DocumentInkBinding.h"

class DocumentInkViewportSync {
public:
    void SetZoom(double zoom);
    void SetPan(double x, double y);
    void SetPage(int page);
    const DocumentInkBinding& Binding() const { return binding_; }
private:
    DocumentInkBinding binding_;
};
