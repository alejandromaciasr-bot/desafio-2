#pragma once
#include <cstddef>

struct Metrics {
    long long iterations = 0;
    std::size_t memoryBytes = 0;
    void reset(){ iterations = 0; memoryBytes = 0; }
};
