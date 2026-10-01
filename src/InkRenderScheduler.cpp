#include "InkRenderScheduler.h"

void InkRenderScheduler::Request() {
    pending_ = true;
    ++generation_;
}

bool InkRenderScheduler::Consume() {
    if (!pending_) return false;
    pending_ = false;
    return true;
}
