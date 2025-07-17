#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"
#include "structs.hpp"
#include <cstddef>

struct DetectedGPUInfo {
    std::string vendor_id;
    std::string device_path;
    std::string card_name;
};

class GpuProvider : public IStatsProvider, ReadProvider {
  private:
    size_t gpu_count = 0;
    std::vector<DetectedGPUInfo> detected_gpus;

  public:
    GpuProvider();
    void update(SystemStats &stats) override;
};
