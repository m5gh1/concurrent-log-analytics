#pragma once
#include "AlertManager.h"
#include "Config.h"
#include "LogAnalytics.h"
#include "LogFilter.h"
#include "LogParser.h"
#include "Metrics.h"
#include "ThreadSafeQueue.h"
#include <atomic>
#include <thread>
#include <vector>

class LogPipeline {
public:
    explicit LogPipeline(Config config);
    bool run();

private:
    Config config_;
};
