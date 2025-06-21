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
        std::cout << stats.cpu.name << std::endl;
        std::cout << "Usage: " << stats.cpu.usage_percent << std::endl;
        std::cout << "Temperature: " << stats.cpu.temperature << std::endl;
        std::cout << "Freq: " << stats.cpu.frequency_mhz << std::endl;
        sleep(1);
    }
}
