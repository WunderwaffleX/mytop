#include "system_monitor.hpp"
#include <unistd.h>

#include "providers/cpu_provider.hpp"

SystemMonitor::SystemMonitor() {
    // TODO: add providers
    providers.push_back(std::make_unique<CpuProvider>());
}

SystemStats SystemMonitor::collect() {
    SystemStats stats;
    stats.timestamp = std::chrono::system_clock::now();

    for (const auto &provider : providers) {
        provider->update(stats);
    }

    return stats;
}
