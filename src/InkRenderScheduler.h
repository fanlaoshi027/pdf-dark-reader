#pragma once

#include <cstdint>

class InkRenderScheduler {
public:
    void Request();
    bool Consume();
    std::uint64_t Generation() const { return generation_; }
private:
    std::uint64_t generation_ = 0;
    bool pending_ = false;
};
