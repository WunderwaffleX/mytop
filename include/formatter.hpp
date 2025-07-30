#pragma once

#include "structs.hpp"
#include <vector>

class Formatter {
  public:
    Formatter(AppState &state);
    void format(SystemStats &stats);
    void sort_processes(std::vector<ProcessInfo> procs);

  private:
    AppState &m_state;

    void formatCPU(CPUStats &cpu);
    void formatMemory(MemoryStats &mem);
    void formatBattery(BatteryStats &bat);
    void formatDisk(std::vector<DiskStats> &disks);
    void formatGPU(std::vector<GPUStats> &gpus);
    void formatProcess(std::vector<ProcessInfo> &procs);
};
