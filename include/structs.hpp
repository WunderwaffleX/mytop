#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

struct CpuTime {
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
    uint64_t total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
    uint64_t idleTime() const {
        return idle + iowait;
    }
};

struct CoreStats {
    size_t coreid;
    float usage_percent;
    float frequency_mhz;
};

struct CPUStats {
    std::string name;
    float usage_percent;
    size_t temperature;
    size_t frequency_mhz;
    std::vector<CpuTime> time;
    std::vector<CpuTime> prev_time;
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

    float total_gb;
    float used_gb;
    float free_gb;
    float available_gb;
    float cached_gb;
    float swap_total_gb;
    float swap_used_gb;
};

struct ProcessInfo {
    size_t pid;
    size_t ppid;
    std::string name;
    size_t user;
    float cpu_usage;
    size_t memory_usage;
    size_t virtual_memory;
    size_t resident_memory;
    float uptime;
    unsigned long time;
    char state;
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
    std::string partition;
    std::string filesystem;
    uint64_t total_space;
    uint64_t used_space;
    float usage_percent;

    float total_space_gb;
    float used_space_gb;
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
    std::vector<GPUStats> gpu;
    MemoryStats memory;
    std::vector<ProcessInfo> processes;
    std::chrono::system_clock::time_point timestamp;
    BatteryStats battery;
    std::vector<DiskStats> disk;
};

enum class SortOrder { PID, NAME, CPU, MEMORY, STATE };

struct AppState {
    std::mutex mutex;
    SystemStats stats;

    std::vector<int> cpu_history = std::vector<int>(100, 0);
    std::vector<int> memory_history = std::vector<int>(100, 0);

    std::vector<size_t> idx_pid;
    std::vector<size_t> idx_name;
    std::vector<size_t> idx_cpu;
    std::vector<size_t> idx_memory;
    std::vector<size_t> idx_state;
};
