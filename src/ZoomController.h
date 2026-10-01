#pragma once

#include "ViewTransformController.h"

class ZoomController {
public:
    explicit ZoomController(ViewTransformController& transform) : transform_(transform) {}

    void Multiply(double factor, double centerX, double centerY);
    void Set(double scale, double centerX, double centerY);

private:
    ViewTransformController& transform_;
};
