#pragma once

#include "ViewTransformState.h"

class ViewTransformController {
public:
    ViewTransformController() = default;

    const ViewTransformState& State() const { return state_; }
    ViewTransformState& State() { return state_; }

    void SetPage(int pageIndex, double width, double height);
    void Pan(double dx, double dy);
    void ZoomAround(double newScale, double screenX, double screenY);
    void SetScale(double scale);
    void ResetOrigin();

private:
    ViewTransformState state_{};
};
