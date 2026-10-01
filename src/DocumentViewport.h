#pragma once

struct ViewPoint {
    double x = 0.0;
    double y = 0.0;
};

class DocumentViewport {
public:
    void SetZoom(double zoom);
    void SetOffset(double x, double y);
    void Pan(double dx, double dy);

    double Zoom() const { return zoom_; }
    ViewPoint Offset() const { return {offsetX_, offsetY_}; }

    ViewPoint DocumentToScreen(ViewPoint document) const;
    ViewPoint ScreenToDocument(ViewPoint screen) const;

private:
    double zoom_ = 1.0;
    double offsetX_ = 0.0;
    double offsetY_ = 0.0;
};
