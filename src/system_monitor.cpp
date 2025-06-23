#include "system_monitor.hpp"
#include <unistd.h>

#include "providers/battery_provider.hpp"
#include "providers/cpu_provider.hpp"
#include "providers/disk_provider.hpp"
#include "providers/memory_provider.hpp"

SystemMonitor::SystemMonitor() {
    // TODO: add providers
    providers.push_back(std::make_unique<CpuProvider>());
    providers.push_back(std::make_unique<MemoryProvider>());
    providers.push_back(std::make_unique<BatteryProvider>());
    providers.push_back(std::make_unique<DiskProvider>());
}

SystemStats SystemMonitor::collect() {
    SystemStats stats;
    stats.timestamp = std::chrono::system_clock::now();

    for (const auto &provider : providers) {
        provider->update(stats);
    }

    return stats;
}
