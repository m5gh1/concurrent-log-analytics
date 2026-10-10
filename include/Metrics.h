#pragma once
#include <chrono>
#include <cstddef>

struct Metrics {
    std::size_t linesRead{0};
    std::size_t accepted{0};
    std::size_t filtered{0};
    std::size_t malformed{0};
    double elapsedSeconds{0.0};

    double throughputPerSecond() const {
        return elapsedSeconds > 0.0
            ? static_cast<double>(accepted) / elapsedSeconds
            : 0.0;
    }
};
