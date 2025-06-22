#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"

#include <string>

struct CpuTime {
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
    uint64_t total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
    uint64_t idleTime() const {
        return idle + iowait;
    }
};

class CpuProvider : public IStatsProvider, ReadProvider {
  private:
    std::string cpuName;
    std::string thermalPath;
    std::string freqPath;

    std::string getCpuNameFromSystem();
    std::string getThermalPath();

    CpuTime parseCpuTimeLine(const std::string &line);
    float getCpuUsage();
    size_t getCpuTemperature();
    float getCpuFreq();

  public:
    CpuProvider();
    void update(SystemStats &stats) override;
};
