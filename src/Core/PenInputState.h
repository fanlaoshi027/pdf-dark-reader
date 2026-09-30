#pragma once

#include <cstdint>

// Lightweight pen state container for low-latency Windows pointer input.
// Keeps input sampling separate from PDF rendering.
class PenInputState {
public:
    void Begin(double x, double y, float pressure) {
        drawing_ = true;
        Update(x, y, pressure);
    }

    void Update(double x, double y, float pressure) {
        x_ = x;
        y_ = y;
        pressure_ = pressure;
        timestamp_++;
    }

    void End() { drawing_ = false; }

    bool drawing() const { return drawing_; }
    double x() const { return x_; }
    double y() const { return y_; }
    float pressure() const { return pressure_; }
    uint64_t timestamp() const { return timestamp_; }

private:
    bool drawing_{false};
    double x_{0.0};
    double y_{0.0};
    float pressure_{0.5f};
    uint64_t timestamp_{0};
};
