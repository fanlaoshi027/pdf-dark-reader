#include "DocumentInkBinding.h"

void DocumentInkBinding::SetMapper(const InkPageCoordinateMapper& mapper) { mapper_ = mapper; }
void DocumentInkBinding::SetPage(int page) { mapper_.anchor.page = page; }
