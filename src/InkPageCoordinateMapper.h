#pragma once
#include "InkPageAnchor.h"

struct InkPageCoordinateMapper {
    InkPageAnchor anchor;
    double zoom = 1.0;
    double panX = 0.0;
    double panY = 0.0;

    double ToViewX(double pageX) const;
    double ToViewY(double pageY) const;
    double ToPageX(double viewX) const;
    double ToPageY(double viewY) const;
};
