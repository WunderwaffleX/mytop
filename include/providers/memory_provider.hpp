#pragma once

#include "iprovider.hpp"
#include "providers/iprovider.hpp"

class MemoryProvider : public IStatsProvider {
  private:
  public:
    MemoryProvider();
    void update(SystemStats &stats) override;
};
