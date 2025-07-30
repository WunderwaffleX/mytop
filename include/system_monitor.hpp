#pragma once

#include <memory>

#include "providers/iprovider.hpp"
#include "structs.hpp"

class SystemMonitor {
  private:
    SystemStats stats;
    std::vector<std::unique_ptr<IStatsProvider>> providers;

  public:
    SystemMonitor();
    SystemStats collect();
};
