#pragma once

#include "ViewTransformController.h"

class PanController {
public:
    explicit PanController(ViewTransformController& transform) : transform_(transform) {}

    void Move(double dx, double dy);

private:
    ViewTransformController& transform_;
};
