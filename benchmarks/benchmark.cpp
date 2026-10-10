#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// A small container/update benchmark, not a substitute for end-to-end pipeline tests.
int main(int argc, char** argv) {
    const std::size_t n = argc > 1 ? std::stoull(argv[1]) : 1'000'000;
    const auto start = std::chrono::steady_clock::now();
    std::unordered_map<std::string, std::size_t> counts;
    for (std::size_t i = 0; i < n; ++i) {
        ++counts["ERROR-" + std::to_string(i % 100)];
    }
    const auto end = std::chrono::steady_clock::now();
    const double seconds = std::chrono::duration<double>(end - start).count();
    std::cout << "Operations: " << n << "\nDistinct keys: " << counts.size()
              << "\nElapsed seconds: " << seconds
              << "\nOperations/sec: " << (seconds > 0 ? n / seconds : 0) << '\n';
}
