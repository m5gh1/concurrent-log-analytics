#include "AlertManager.h"
#include <fstream>

AlertManager::AlertManager(std::size_t threshold) : threshold_(threshold) {}

std::vector<Alert> AlertManager::evaluate(const AnalyticsSnapshot& snapshot) const {
    std::vector<Alert> alerts;
    for (const auto& item : snapshot.errorCounts) {
        if (item.second >= threshold_) alerts.push_back({item.first, item.second});
    }
    return alerts;
}

bool AlertManager::writeToFile(const std::vector<Alert>& alerts, const std::string& path) const {
    std::ofstream output(path);
    if (!output) return false;

    if (alerts.empty()) {
        output << "No threshold exceeded\n";
    } else {
        output << "ALERTS (threshold: " << threshold_ << " occurrences)\n";
        for (const auto& alert : alerts) {
            output << "[ERROR] \"" << alert.message << "\" occurred "
                   << alert.occurrences << " times\n";
        }
    }
    return static_cast<bool>(output);
}
