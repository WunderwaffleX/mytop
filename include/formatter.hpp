#pragma once

#include "structs.hpp"
#include <vector>

class Formatter {
  public:
    Formatter(AppState &state);
    void format(SystemStats &stats);
    void sort_processes(std::vector<ProcessInfo> procs);

  private:
    AppState &state_;

    void format_cpu(CPUStats &cpu);
    void format_memory(MemoryStats &mem);
    void format_battery(BatteryStats &bat);
    void format_disk(std::vector<DiskStats> &disks);
    void format_gpu(std::vector<GPUStats> &gpus);
    void format_process(std::vector<ProcessInfo> &procs);
};
