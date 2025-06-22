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
        std::cout << "Total: " << stats.memory.total << std::endl;
        std::cout << "Used: " << stats.memory.used_percent << '%' << std::endl;
        std::cout << "Used: " << stats.memory.used << std::endl;
        std::cout << "Free: " << stats.memory.free << std::endl;
        std::cout << "Available: " << stats.memory.available << std::endl;
        std::cout << "Cached: " << stats.memory.cached << std::endl;
        std::cout << "Swap Total: " << stats.memory.swap_total << std::endl;
        std::cout << "Swap Used: " << stats.memory.swap_used << std::endl;
        sleep(1);
    }
}
