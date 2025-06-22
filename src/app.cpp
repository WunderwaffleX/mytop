#include "app.hpp"
#include "system_monitor.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

void App::run() {
    SystemMonitor monitor;
    std::string delim(12, '-');
    while (true) {
        std::cout << "\033c";
        SystemStats stats = monitor.collect();
        std::cout << delim << "CPU" << delim << std::endl;
        std::cout << "Name: " << stats.cpu.name << std::endl;
        std::cout << "Usage: " << stats.cpu.usage_percent << std::endl;
        std::cout << "Temperature: " << stats.cpu.temperature << std::endl;
        std::cout << "Freq: " << stats.cpu.frequency_mhz << std::endl;

        std::cout << delim << "Memory" << delim << std::endl;
        std::cout << "Total: " << stats.memory.total << " kb" << std::endl;
        std::cout << "Used: " << stats.memory.used_percent << '%' << std::endl;
        std::cout << "Used: " << stats.memory.used << " kb" << std::endl;
        std::cout << "Free: " << stats.memory.free << " kb" << std::endl;
        std::cout << "Available: " << stats.memory.available << " kb"
                  << std::endl;
        std::cout << "Cached: " << stats.memory.cached << " kb" << std::endl;
        std::cout << "Swap Total: " << stats.memory.swap_total << " kb"
                  << std::endl;
        std::cout << "Swap Used: " << stats.memory.swap_used << " kb"
                  << std::endl;

        std::cout << delim << "Battery" << delim << std::endl;
        std::cout << "Status: " << stats.battery.status << std::endl;
        std::cout << "Charge now: " << stats.battery.charge_now << std::endl;
        std::cout << "Charge full: " << stats.battery.charge_full << std::endl;
        std::cout << "Current now: " << stats.battery.current_now << std::endl;
        std::cout << "Percentage: " << stats.battery.percentage << '%'
                  << std::endl;
        std::cout << "Time remaining: " << stats.battery.time_remaining
                  << std::endl;

        sleep(1);
    }
}
