#include "formatter.hpp"
#include "structs.hpp"
#include <numeric>
#include <vector>

Formatter::Formatter(AppState &state) : m_state(state) {};

void Formatter::format(SystemStats &stats) {
    formatCPU(stats.cpu);
    formatMemory(stats.memory);
    formatBattery(stats.battery);
    formatDisk(stats.disk);
    formatGPU(stats.gpu);
    formatProcess(stats.processes);
}

void Formatter::sort_processes(std::vector<ProcessInfo> procs) {
    formatProcess(procs);
}

void Formatter::formatCPU(CPUStats &cpu) {
    cpu.frequency_mhz /= 1000;
    for (CoreStats &core : cpu.cores) {
        core.frequency_mhz /= 1000;
    }
};

void Formatter::formatMemory(MemoryStats &mem) {
    auto to_gb = [](double bytes) { return bytes / 1e6; };
    mem.total_gb = to_gb(mem.total);
    mem.used_gb = to_gb(mem.used);
    mem.free_gb = to_gb(mem.free);
    mem.available_gb = to_gb(mem.available);
    mem.cached_gb = to_gb(mem.cached);
    mem.swap_total_gb = to_gb(mem.swap_total);
    mem.swap_used_gb = to_gb(mem.swap_used);
};
void Formatter::formatBattery(BatteryStats &bat) {};
void Formatter::formatDisk(std::vector<DiskStats> &disks) {
    for (DiskStats &disk : disks) {
        disk.total_space_gb = disk.total_space / 1e9;
        disk.used_space_gb = disk.used_space / 1e9;
    }
};
void Formatter::formatGPU(std::vector<GPUStats> &gpus) {

};
void Formatter::formatProcess(std::vector<ProcessInfo> &processes) {
    size_t n = processes.size();
    std::vector<size_t> base_idx(n);
    std::iota(base_idx.begin(), base_idx.end(), 0);

    auto sort_indices = [&](std::vector<size_t> &indices, auto comp) {
        indices = base_idx;
        std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
            return comp(processes[a], processes[b]);
        });
    };

    sort_indices(m_state.idx_pid,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.pid < b.pid;
                 });

    sort_indices(m_state.idx_name,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.name < b.name;
                 });

    sort_indices(m_state.idx_cpu,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.cpu_usage > b.cpu_usage; // по убыванию
                 });

    sort_indices(
        m_state.idx_memory, [&](const ProcessInfo &a, const ProcessInfo &b) {
            return a.resident_memory > b.resident_memory; // по убыванию
        });

    sort_indices(m_state.idx_state,
                 [&](const ProcessInfo &a, const ProcessInfo &b) {
                     return a.state < b.state;
                 });
};
