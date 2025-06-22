#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"
#include <cstddef>

class BatteryProvider : public IStatsProvider, ReadProvider {
  private:
    std::string battery_path;

    std::string getBatteryPath();

    std::string getBatteryStatus();

    size_t getBatteryChargeNow();
    size_t getBatteryChargeFull();
    size_t getBatteryCurrentNow();

    size_t getBatteryPercentage(size_t charge_now, size_t charge_full);
    float getBatteryTimeRemaining(std::string status, size_t charge_now,
                                  size_t charge_full, size_t current_now);

  public:
    BatteryProvider();
    void update(SystemStats &stats) override;
};
