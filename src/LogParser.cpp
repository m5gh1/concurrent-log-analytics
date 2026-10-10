#include "LogParser.h"
#include <sstream>

std::optional<LogEntry> LogParser::parse(const std::string& line) const {
    if (line.empty()) return std::nullopt;

    std::istringstream input(line);
    std::string date, time, levelText;

    if (!(input >> date >> time >> levelText)) return std::nullopt;
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return std::nullopt;
    if (time.size() != 8 || time[2] != ':' || time[5] != ':') return std::nullopt;

    LogLevel level;
    if (levelText == "DEBUG") level = LogLevel::DEBUG;
    else if (levelText == "INFO") level = LogLevel::INFO;
    else if (levelText == "WARN") level = LogLevel::WARN;
    else if (levelText == "ERROR") level = LogLevel::ERROR;
    else return std::nullopt;

    std::string message;
    std::getline(input >> std::ws, message);
    if (message.empty()) return std::nullopt;

    return LogEntry{date + " " + time, level, message};
}
