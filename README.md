# Concurrent Log Processing and Alerting Platform

A C++17 command-line application that reads log files line by line, processes records using a bounded producer-consumer queue and worker threads, aggregates severity/error statistics, reports frequent errors, and writes threshold-based alerts.

## Features

- Line-by-line file ingestion (does not load the entire input file into memory).
- Parser for `YYYY-MM-DD HH:MM:SS LEVEL message` records.
- Bounded blocking queue with mutexes, condition variables, and graceful close/drain behavior.
- Configurable worker count and queue capacity.
- Thread-safe analytics for severity and error-message counts.
- Top-K error report and threshold-based alert output.
- Basic message substring filter.
- CMake build, dependency-free tests, GitHub Actions CI, and a small container benchmark.

## Requirements

- C++17 compiler (GCC 8+ or a compatible compiler)
- CMake 3.16+
- pthreads / POSIX thread support (normally available on Linux)

## Build on Linux / WSL Ubuntu

```bash
sudo apt update
sudo apt install -y build-essential cmake git
git init
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Run

From the repository root:

```bash
./build/log_analytics
./build/log_analytics --input data/sample.log --workers 4 --queue-capacity 1024 --top-k 5 --threshold 3
./build/log_analytics --contains Database
./build/log_analytics --alerts output/alerts.txt --help
```

Options:
- `--input PATH`: input file (default `data/sample.log`)
- `--alerts PATH`: output file for alerts (default `output/alerts.txt`)
- `--workers N`: worker thread count (default `4`)
- `--queue-capacity N`: maximum pending tasks (default `1024`)
- `--top-k N`: number of frequent error messages to report (default `5`)
- `--threshold N`: alert when an error message reaches this count (default `3`)
- `--contains TEXT`: only include messages containing this substring

The parser accepts the levels `DEBUG`, `INFO`, `WARN`, and `ERROR`. Invalid records are counted as malformed and skipped. The parser validates the timestamp's basic shape, not calendar correctness.

## Design notes

- The main thread is the producer: it reads, parses, filters, and submits records.
- Worker threads consume tasks and update analytics.
- The queue's mutex protects queue state only; workers release it before processing records.
- `condition_variable` allows workers to sleep while the queue is empty. A bounded queue also blocks the producer when full.
- `close()` prevents new pushes, wakes all waiters, and lets workers drain pending tasks before exiting.
- Analytics uses a mutex-protected `unordered_map`. This is simple and correct, though it may create contention. Per-worker counters followed by a merge are a possible optimization to benchmark.
- Top-K is calculated after processing from the error-frequency table.
- The program uses a fixed worker count supplied by configuration; four workers are a demo default, not a claim that four is always optimal.

## Performance claims

Do not claim a specific speedup until you benchmark it. Compare single-threaded and multi-threaded runs on the same machine and input, repeat runs, and report throughput in records/second. File I/O, parsing cost, queue capacity, lock contention, and CPU core count all affect results. The included `container_benchmark` measures a container-update loop only; it is not an end-to-end pipeline benchmark.

## Limitations / future improvements

- Input is currently a file, not a live stream or network service.
- Alert rules are simple frequency thresholds; there is no time-windowed alerting yet.
- Message grouping is exact-string matching; normalization can be added for IDs and variable values.
- Timestamp validation checks format shape but not full date/time validity.
- The current shared analytics mutex is correct but can limit scaling.
- No persistent database, HTTP API, or automatic log rotation is included.

## Suggested next steps

1. Add more parser edge-case tests.
2. Add end-to-end tests for multiple worker counts and malformed records.
3. Add ThreadSanitizer and AddressSanitizer build presets.
4. Implement a repeatable end-to-end benchmark with generated input files.
5. Measure and document the real results before updating resume percentages.
