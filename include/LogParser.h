#pragma once
#include "LogEntry.h"
#include <optional>
#include <string>

class LogParser {
public:
    // Expected format: YYYY-MM-DD HH:MM:SS LEVEL message
    std::optional<LogEntry> parse(const std::string& line) const;
};
