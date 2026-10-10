#pragma once
#include "LogEntry.h"
#include <optional>
#include <string>

struct LogFilter {
    std::optional<LogLevel> minimumLevel;
    std::string messageContains;

    bool matches(const LogEntry& entry) const;
};
