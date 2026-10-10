#pragma once
#include "LogEntry.h"
#include <cstddef>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct ErrorFrequency {
    std::string message;
    std::size_t count{};
};

struct AnalyticsSnapshot {
    std::size_t processed{0};
    std::size_t malformed{0};
    std::unordered_map<std::string, std::size_t> levelCounts;
    std::unordered_map<std::string, std::size_t> errorCounts;
    std::vector<ErrorFrequency> topErrors;
};

class LogAnalytics {
public:
    void recordProcessed(const LogEntry& entry);
    void recordMalformed();
    AnalyticsSnapshot snapshot(std::size_t topK) const;

private:
    mutable std::mutex mutex_;
    std::size_t processed_{0};
    std::size_t malformed_{0};
    std::unordered_map<std::string, std::size_t> levelCounts_;
    std::unordered_map<std::string, std::size_t> errorCounts_;
};
