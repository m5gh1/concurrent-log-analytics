#include "LogAnalytics.h"
#include <algorithm>
#include <queue>
#include <utility>

void LogAnalytics::recordProcessed(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    ++processed_;
    ++levelCounts_[toString(entry.level)];
    if (entry.level == LogLevel::ERROR) ++errorCounts_[entry.message];
}

void LogAnalytics::recordMalformed() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++malformed_;
}

AnalyticsSnapshot LogAnalytics::snapshot(std::size_t topK) const {
    std::lock_guard<std::mutex> lock(mutex_);
    AnalyticsSnapshot result;
    result.processed = processed_;
    result.malformed = malformed_;
    result.levelCounts = levelCounts_;
    result.errorCounts = errorCounts_;

    // Keep the K most frequent errors. Ties are resolved by message for stable output.
    auto compare = [](const ErrorFrequency& a, const ErrorFrequency& b) {
        if (a.count != b.count) return a.count < b.count;
        return a.message > b.message;
    };
    std::priority_queue<ErrorFrequency, std::vector<ErrorFrequency>, decltype(compare)>
        heap(compare);

    for (const auto& item : errorCounts_) {
        heap.push(ErrorFrequency{item.first, item.second});
    }
    while (!heap.empty() && result.topErrors.size() < topK) {
        result.topErrors.push_back(heap.top());
        heap.pop();
    }
    return result;
}
