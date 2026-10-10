#pragma once
#include <string>

enum class LogLevel { DEBUG, INFO, WARN, ERROR, UNKNOWN };

struct LogEntry {
    std::string timestamp;
    LogLevel level{LogLevel::UNKNOWN};
    std::string message;
};

std::string toString(LogLevel level);
