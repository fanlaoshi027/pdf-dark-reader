#include "LayerInput.h"

bool LayerInput::BeginPen(LayerSystem& layers, POINT viewPoint, float pressure) {
    // Pressure is kept in the public API now; Windows pen integration can feed
    // native pressure later without changing the document model.
    (void)pressure;
    POINT pdf = layers.ViewToPdf(viewPoint.x, viewPoint.y);
    layers.SetPenEnabled(true);
    return pdf.x >= 0 || pdf.y >= 0;
}

bool LayerInput::UpdatePen(LayerSystem& layers, POINT viewPoint, float pressure) {
    (void)layers; (void)viewPoint; (void)pressure;
    return true;
}

bool LayerInput::EndPen(LayerSystem& layers, POINT viewPoint, float pressure) {
    (void)layers; (void)viewPoint; (void)pressure;
    return true;
}

bool LayerInput::Erase(LayerSystem& layers, POINT viewPoint, int radius) {
    (void)layers; (void)viewPoint; (void)radius;
    return true;
}

bool LayerInput::AddLassoPoint(LayerSystem& layers, POINT viewPoint) {
    (void)layers; (void)viewPoint;
    return true;
}

bool LayerInput::FinishLasso(LayerSystem& layers) {
    (void)layers;
    return true;
}
