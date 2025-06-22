#pragma once

#include <chrono>
#include <string>
#include <vector>

struct CoreStats {
    size_t coreid;
    float usage_percent;
    size_t temperature;
    float frequency_mhz;
};

struct CPUStats {
    std::string name;
    float usage_percent;
    size_t temperature;
    size_t frequency_mhz;
    std::vector<CoreStats> cores;
};

struct MemoryStats {
    size_t total;
    size_t used;
    float used_percent;
    size_t free;
    size_t available;
    size_t cached;
    size_t swap_total;
    size_t swap_used;
};

struct ProcessInfo {
    size_t pid;
    std::string name;
    std::string user;
    float cpu_usage;
    size_t memory_usage;
    size_t virtual_memory;
    size_t resident_memory;
    float uptime;
    std::string state;
    size_t nice;
    size_t priority;
    size_t threads;
    std::string command;
};

struct BatteryStats {
    std::string status;
    size_t charge_now;
    size_t charge_full;
    size_t current_now;
    size_t percentage;
    float time_remaining;
};

struct DiskStats {
    std::string name;
    uint64_t total_space;
    uint64_t used_space;
    float usage_percent;
};

struct GPUStats {
    std::string name;
    float usage_percent;
    size_t temperature;
    size_t memory_total;
    size_t memory_used;
};

struct SystemStats {
    CPUStats cpu;
    GPUStats gpu;
    MemoryStats memory;
    std::vector<ProcessInfo> processes;
    std::chrono::system_clock::time_point timestamp;
    BatteryStats battery;
    DiskStats disk;
};
