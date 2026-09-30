#pragma once

// Low-latency pen pipeline tuning values.
// Kept separate so rendering and input tuning can evolve independently.

namespace PenLatencyConfig {

constexpr int kInputBatchSize = 1;
constexpr float kMinPressure = 0.05f;
constexpr float kMaxPressure = 1.0f;
constexpr float kSmoothingFactor = 0.18f;
constexpr bool kPreferPenPointer = true;

}
