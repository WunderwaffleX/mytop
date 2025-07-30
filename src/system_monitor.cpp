#include "system_monitor.hpp"
#include <memory>
#include <unistd.h>

#include "providers/battery_provider.hpp"
#include "providers/cpu_provider.hpp"
#include "providers/disk_provider.hpp"
#include "providers/gpu_provider.hpp"
#include "providers/memory_provider.hpp"
#include "providers/process_provider.hpp"
#include "structs.hpp"

SystemMonitor::SystemMonitor() {
    providers.push_back(std::make_unique<CpuProvider>());
    providers.push_back(std::make_unique<MemoryProvider>());
    providers.push_back(std::make_unique<BatteryProvider>());
    providers.push_back(std::make_unique<DiskProvider>());
    providers.push_back(std::make_unique<GpuProvider>());
    providers.push_back(std::make_unique<ProcessProvider>());
}

SystemStats SystemMonitor::collect() {
    m_stats.timestamp = std::chrono::system_clock::now();

    for (size_t i = 0; i < providers.size(); ++i) {
        try {
            providers[i]->update(m_stats);
        } catch (const std::exception &e) {
            std::cerr << "Provider " << i << " threw: " << e.what()
                      << std::endl;
        } catch (...) {
            std::cerr << "Provider " << i << " threw unknown error"
                      << std::endl;
        }
    }
    return m_stats;
}
