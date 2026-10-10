#include "Config.h"
#include <iostream>
#include <stdexcept>

namespace {
std::size_t parsePositive(const std::string& value, const std::string& option) {
    try {
        std::size_t used = 0;
        auto result = std::stoull(value, &used);
        if (used != value.size() || result == 0) throw std::invalid_argument("invalid");
        return static_cast<std::size_t>(result);
    } catch (...) {
        throw std::invalid_argument("Invalid positive integer for " + option + ": " + value);
    }
}
}

bool parseArgs(int argc, char* argv[], Config& config) {
    try {
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            auto value = [&](const std::string& name) -> std::string {
                if (i + 1 >= argc) throw std::invalid_argument("Missing value for " + name);
                return argv[++i];
            };
            if (arg == "--input") config.inputPath = value(arg);
            else if (arg == "--alerts") config.alertOutputPath = value(arg);
            else if (arg == "--workers") config.workers = parsePositive(value(arg), arg);
            else if (arg == "--queue-capacity") config.queueCapacity = parsePositive(value(arg), arg);
            else if (arg == "--top-k") config.topK = parsePositive(value(arg), arg);
            else if (arg == "--threshold") config.alertThreshold = parsePositive(value(arg), arg);
            else if (arg == "--contains") config.messageContains = value(arg);
            else if (arg == "--help") {
                std::cout
                    << "Usage: log_analytics [options]\n"
                    << "  --input PATH           Input log file (default data/sample.log)\n"
                    << "  --alerts PATH          Alert output (default output/alerts.txt)\n"
                    << "  --workers N            Worker count (default 4)\n"
                    << "  --queue-capacity N     Bounded queue capacity (default 1024)\n"
                    << "  --top-k N              Number of frequent errors to show (default 5)\n"
                    << "  --threshold N          Alert threshold (default 3)\n"
                    << "  --contains TEXT        Only process messages containing TEXT\n";
                return false;
            } else {
                throw std::invalid_argument("Unknown option: " + arg);
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Argument error: " << error.what() << '\n';
        return false;
    }
    return true;
}
