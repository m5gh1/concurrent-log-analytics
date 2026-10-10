#include "AlertManager.h"
#include <cassert>
#include <iostream>

int main() {
    AnalyticsSnapshot snapshot;
    snapshot.errorCounts["Database failed"] = 3;
    snapshot.errorCounts["Timeout"] = 2;
    AlertManager manager(3);
    const auto alerts = manager.evaluate(snapshot);
    assert(alerts.size() == 1);
    assert(alerts[0].message == "Database failed");
    assert(alerts[0].occurrences == 3);
    std::cout << "Alert tests passed\n";
}
