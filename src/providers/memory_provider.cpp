#include "providers/memory_provider.hpp"
#include "structs.hpp"

#include <fstream>
#include <sstream>

#include <unistd.h>
#include <unordered_map>

MemoryProvider::MemoryProvider() {}

void MemoryProvider::update(SystemStats &stats) {

    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    std::unordered_map<std::string, size_t> mem_data;

    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        size_t value;
        std::string unit;
        iss >> key >> value >> unit;

        if (!key.empty() && key.back() == ':')
            key.pop_back();

        mem_data[key] = value;
    }

    size_t total = mem_data["MemTotal"];
    size_t free = mem_data["MemFree"];
    size_t cached = mem_data["Cached"];
    size_t available = mem_data["MemAvailable"];
    size_t swap_total = mem_data["SwapTotal"];
    size_t swap_free = mem_data["SwapFree"];
    size_t used = total - free - cached;
    float used_percent = ((float)used / (float)total) * 100;

    stats.memory.total = total;
    stats.memory.free = free;
    stats.memory.available = available;
    stats.memory.cached = cached;
    stats.memory.used = used;
    stats.memory.used_percent = used_percent;
    stats.memory.swap_total = swap_total;
    stats.memory.swap_used = swap_total - swap_free;
}

// struct MemoryStats {
//     size_t total;
//     size_t used;
//     size_t free;
//     size_t available;
//     size_t cached;
//     size_t swap_total;
//     size_t swap_used;
// };
