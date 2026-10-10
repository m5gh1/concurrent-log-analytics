#include "ThreadSafeQueue.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

int main() {
    ThreadSafeQueue<int> queue(8);
    constexpr int total = 10000;
    std::atomic<int> sum{0};
    std::thread consumer([&] {
        while (true) {
            auto value = queue.pop();
            if (!value) break;
            sum += *value;
        }
    });
    for (int i = 1; i <= total; ++i) assert(queue.push(i));
    queue.close();
    consumer.join();
    assert(sum == (total * (total + 1)) / 2);
    assert(!queue.push(1));
    std::cout << "Queue tests passed\n";
}
