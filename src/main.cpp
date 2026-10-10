#include "Config.h"
#include "LogPipeline.h"
#include <iostream>

// Defined in Config.cpp
bool parseArgs(int argc, char* argv[], Config& config);

int main(int argc, char* argv[]) {
    Config config;
    if (!parseArgs(argc, argv, config)) return (argc > 1 && std::string(argv[1]) == "--help") ? 0 : 2;

    try {
        LogPipeline pipeline(config);
        return pipeline.run() ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
}
