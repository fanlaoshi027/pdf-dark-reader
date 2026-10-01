#include "PanController.h"

void PanController::Move(double dx, double dy) {
    transform_.Pan(dx, dy);
}
