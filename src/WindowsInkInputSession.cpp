#include "WindowsInkInputSession.h"

bool WindowsInkInputSession::Begin(HWND hwnd, UINT32 pointerId, InkSample& sample) {
    if (!adapter_.Begin(hwnd, pointerId, sample)) return false;
    hwnd_ = hwnd;
    pointerId_ = pointerId;
    active_ = capture_.Capture(hwnd, pointerId);
    return active_;
}

bool WindowsInkInputSession::Update(HWND hwnd, UINT32 pointerId, std::vector<InkSample>& samples) {
    if (!active_ || pointerId != pointerId_) return false;
    if (history_.Read(pointerId, samples)) return true;
    InkSample sample{};
    if (!adapter_.Update(hwnd, pointerId, sample)) return false;
    samples.clear();
    samples.push_back(sample);
    return true;
}

bool WindowsInkInputSession::End(HWND hwnd, UINT32 pointerId, InkSample& sample) {
    if (!active_ || pointerId != pointerId_) return false;
    const bool ok = adapter_.End(hwnd, pointerId, sample);
    capture_.Release(hwnd_, pointerId_);
    Reset();
    return ok;
}

void WindowsInkInputSession::Reset() {
    hwnd_ = nullptr;
    pointerId_ = 0;
    active_ = false;
}
