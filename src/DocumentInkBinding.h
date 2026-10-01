#pragma once
#include "InkPageCoordinateMapper.h"

class DocumentInkBinding {
public:
    void SetMapper(const InkPageCoordinateMapper& mapper);
    const InkPageCoordinateMapper& Mapper() const { return mapper_; }
    void SetPage(int page);
    int Page() const { return mapper_.anchor.page; }
private:
    InkPageCoordinateMapper mapper_;
};
