#include "formatter.hpp"
#include "structs.hpp"
#include <numeric>
#include <vector>

Formatter::Formatter(AppState &state) : state_(state) {};

void Formatter::format(SystemStats &stats) {
    format_cpu(stats.cpu);
    format_memory(stats.memory);
    format_battery(stats.battery);
    format_disk(stats.disk);
    format_gpu(stats.gpu);
    format_process(stats.processes);
}

void Formatter::sort_processes(std::vector<ProcessInfo> procs) {
    format_process(procs);
}

void Formatter::format_cpu(CPUStats &cpu) {
    cpu.frequency_mhz /= 1000;
    for (CoreStats &core : cpu.cores) {
        core.frequency_mhz /= 1000;
    }
};

void Formatter::format_memory(MemoryStats &mem) {
    auto to_gb = [](double bytes) { return bytes / 1e6; };
    mem.total_gb = to_gb(mem.total);
    mem.used_gb = to_gb(mem.used);
    mem.free_gb = to_gb(mem.free);
    mem.available_gb = to_gb(mem.available);
    mem.cached_gb = to_gb(mem.cached);
    mem.swap_total_gb = to_gb(mem.swap_total);
    mem.swap_used_gb = to_gb(mem.swap_used);
};
void Formatter::format_battery(BatteryStats &bat) {};
void Formatter::format_disk(std::vector<DiskStats> &disks) {
    for (DiskStats &disk : disks) {
        disk.total_space_gb = disk.total_space / 1e9;
        disk.used_space_gb = disk.used_space / 1e9;
    }
};
void Formatter::format_gpu(std::vector<GPUStats> &gpus) {

};
void Formatter::format_process(std::vector<ProcessInfo> &processes) {
    size_t n = processes.size();
    std::vector<size_t> base_idx(n);
    std::iota(base_idx.begin(), base_idx.end(), 0);

    auto sort_indices = [&](std::vector<size_t> &indices, auto comp) {
        indices = base_idx;
        std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
            return comp(processes[a], processes[b]);
        });
    };

    sort_indices(state_.idx_pid,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.pid < b.pid;
                 });

    sort_indices(state_.idx_name,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.name < b.name;
                 });

    sort_indices(state_.idx_cpu,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.cpu_usage > b.cpu_usage; // по убыванию
                 });

    sort_indices(
        state_.idx_memory, [&](const ProcessInfo &a, const ProcessInfo &b) {
            return a.resident_memory > b.resident_memory; // по убыванию
        });

    sort_indices(state_.idx_state,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.state < b.state;
                 });
};
