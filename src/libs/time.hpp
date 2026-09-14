#ifndef CONWAY_TIME_HPP
#define CONWAY_TIME_HPP

#include <cstdint>
#include <chrono>

namespace Time {
    inline uint64_t getNanoS()
    {
        auto tse = std::chrono::steady_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(tse).count();
    }
}

#endif