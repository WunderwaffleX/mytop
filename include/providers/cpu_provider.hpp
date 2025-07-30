#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"
#include "structs.hpp"

#include <string>

class CpuProvider : public IStatsProvider, ReadProvider {
  private:
    std::string cpuName;
    int coresAmount = 0;
    std::string thermalPath;
    std::string freqPath;

    std::string getCpuNameFromSystem();
    int getCoresAmount();
    std::string getThermalPath();

    CpuTime parseCpuTimeLine(const std::string &line);
    std::vector<CpuTime> getCpuTimeSnapshot();
    float getCpuUsage(const CpuTime &t1, const CpuTime &t2);
    size_t getCpuTemperature();
    float getCpuFreq();
    std::vector<CoreStats> getCoreStats(std::vector<CpuTime> t1,
                                        std::vector<CpuTime> t2);

  public:
    CpuProvider();
    void update(SystemStats &stats) override;
};
