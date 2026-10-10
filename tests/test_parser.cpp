#include "LogParser.h"
#include "LogEntry.h"
#include <cassert>
#include <iostream>

int main() {
    LogParser parser;
    auto valid = parser.parse("2026-10-10 10:00:01 ERROR Database connection failed");
    assert(valid.has_value());
    assert(valid->timestamp == "2026-10-10 10:00:01");
    assert(valid->level == LogLevel::ERROR);
    assert(valid->message == "Database connection failed");
    assert(!parser.parse(""));
    assert(!parser.parse("not a valid log line"));
    assert(!parser.parse("2026-10-10 10:00:01 TRACE unknown level"));
    assert(!parser.parse("2026-10-10 10:00:01 INFO"));
    std::cout << "Parser tests passed\n";
}
