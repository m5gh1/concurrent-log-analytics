#pragma once
#include "LogAnalytics.h"
#include <cstddef>
#include <string>
#include <vector>

struct Alert {
    std::string message;
    std::size_t occurrences{};
};

class AlertManager {
public:
    explicit AlertManager(std::size_t threshold);
    std::vector<Alert> evaluate(const AnalyticsSnapshot& snapshot) const;
    bool writeToFile(const std::vector<Alert>& alerts, const std::string& path) const;

private:
    std::size_t threshold_;
};
