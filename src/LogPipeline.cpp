#include "LogPipeline.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>

LogPipeline::LogPipeline(Config config) : config_(std::move(config)) {}

bool LogPipeline::run() {
    std::ifstream input(config_.inputPath);
    if (!input) {
        std::cerr << "Could not open input file: " << config_.inputPath << '\n';
        return false;
    }

    std::error_code fsError;
    const auto parent = std::filesystem::path(config_.alertOutputPath).parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent, fsError);
    if (fsError) {
        std::cerr << "Could not create output directory: " << fsError.message() << '\n';
        return false;
    }

    using Task = std::optional<LogEntry>; // nullopt represents a malformed record
    ThreadSafeQueue<Task> queue(config_.queueCapacity);
    LogParser parser;
    LogFilter filter;
    filter.messageContains = config_.messageContains;
    LogAnalytics analytics;
    Metrics metrics;
    std::atomic<std::size_t> filtered{0};
    std::vector<std::thread> workers;
    workers.reserve(config_.workers);

    const auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < config_.workers; ++i) {
        workers.emplace_back([&]() {
            while (true) {
                auto task = queue.pop();
                if (!task) break;                 // queue closed and drained
                if (!task->has_value()) {
                    analytics.recordMalformed();
                    continue;
                }
                const LogEntry& entry = **task;
                if (!filter.matches(entry)) {
                    ++filtered;
                    continue;
                }
                analytics.recordProcessed(entry);
            }
        });
    }

    std::string line;
    while (std::getline(input, line)) {
        ++metrics.linesRead;
        auto parsed = parser.parse(line);
        if (!parsed) {
            queue.push(Task{});
            continue;
        }
        if (!filter.matches(*parsed)) {
            ++filtered;
            continue;
        }
        if (queue.push(Task{std::move(parsed)})) {
            ++metrics.accepted;
        } else {
            std::cerr << "Queue closed before input was fully submitted\n";
            break;
        }
    }

    queue.close();
    for (auto& worker : workers) {
        if (worker.joinable()) worker.join();
    }

    metrics.filtered = filtered.load();
    const auto end = std::chrono::steady_clock::now();
    metrics.elapsedSeconds = std::chrono::duration<double>(end - start).count();

    const auto report = analytics.snapshot(config_.topK);
    // Malformed lines are counted by workers; accepted counts valid records submitted,
    // while processed counts records actually accepted by the analytics filter.
    metrics.malformed = report.malformed;

    std::cout << "\n=== Concurrent Log Analytics Report ===\n";
    std::cout << "Input: " << config_.inputPath << '\n'
              << "Lines read: " << metrics.linesRead << '\n'
              << "Valid records submitted: " << metrics.accepted << '\n'
              << "Records processed: " << report.processed << '\n'
              << "Malformed records: " << metrics.malformed << '\n'
              << "Filtered records: " << metrics.filtered << '\n'
              << "Workers: " << config_.workers << '\n'
              << "Elapsed seconds: " << std::fixed << std::setprecision(4)
              << metrics.elapsedSeconds << '\n'
              << "Throughput (processed records/sec): "
              << std::setprecision(2) << metrics.throughputPerSecond() << "\n\n";

    std::cout << "Level counts:\n";
    for (const auto& item : report.levelCounts) {
        std::cout << "  " << item.first << ": " << item.second << '\n';
    }
    std::cout << "\nTop errors:\n";
    if (report.topErrors.empty()) std::cout << "  No errors found\n";
    for (std::size_t i = 0; i < report.topErrors.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << report.topErrors[i].message
                  << " - " << report.topErrors[i].count << " occurrences\n";
    }

    AlertManager alertManager(config_.alertThreshold);
    const auto alerts = alertManager.evaluate(report);
    if (!alertManager.writeToFile(alerts, config_.alertOutputPath)) {
        std::cerr << "Could not write alerts to: " << config_.alertOutputPath << '\n';
        return false;
    }
    std::cout << "\nAlerts written to: " << config_.alertOutputPath << '\n';
    std::cout << "Alert count: " << alerts.size() << '\n';
    return true;
}
