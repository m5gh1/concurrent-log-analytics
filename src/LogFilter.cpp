#include "LogFilter.h"

namespace {
int rank(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return 0;
        case LogLevel::INFO: return 1;
        case LogLevel::WARN: return 2;
        case LogLevel::ERROR: return 3;
        default: return -1;
    }
}
}

bool LogFilter::matches(const LogEntry& entry) const {
    if (minimumLevel && rank(entry.level) < rank(*minimumLevel)) return false;
    if (!messageContains.empty() &&
        entry.message.find(messageContains) == std::string::npos) return false;
    return true;
}
