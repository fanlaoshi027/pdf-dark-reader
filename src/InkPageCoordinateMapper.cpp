#include "InkPageCoordinateMapper.h"

double InkPageCoordinateMapper::ToViewX(double pageX) const { return pageX * zoom + panX; }
double InkPageCoordinateMapper::ToViewY(double pageY) const { return pageY * zoom + panY; }
double InkPageCoordinateMapper::ToPageX(double viewX) const { return (viewX - panX) / zoom; }
double InkPageCoordinateMapper::ToPageY(double viewY) const { return (viewY - panY) / zoom; }
