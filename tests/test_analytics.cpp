#include "LogAnalytics.h"
#include <cassert>
#include <iostream>

int main() {
    LogAnalytics analytics;
    analytics.recordProcessed({"2026-10-10 10:00:00", LogLevel::ERROR, "DB failed"});
    analytics.recordProcessed({"2026-10-10 10:00:01", LogLevel::ERROR, "DB failed"});
    analytics.recordProcessed({"2026-10-10 10:00:02", LogLevel::INFO, "Started"});
    auto result = analytics.snapshot(1);
    assert(result.processed == 3);
    assert(result.levelCounts.at("ERROR") == 2);
    assert(result.levelCounts.at("INFO") == 1);
    assert(result.topErrors.size() == 1);
    assert(result.topErrors[0].message == "DB failed");
    assert(result.topErrors[0].count == 2);
    std::cout << "Analytics tests passed\n";
}
