#pragma once

#include "structs.hpp"

class IStatsProvider {
  public:
    virtual void update(SystemStats &stats) = 0;
    virtual ~IStatsProvider() = default;
};
