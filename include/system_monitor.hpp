#pragma once

#include <memory>

#include "providers/iprovider.hpp"

class SystemMonitor {

  public:
    SystemMonitor();
    SystemStats collect();

  private:
    std::vector<std::unique_ptr<IStatsProvider>> providers;
};
