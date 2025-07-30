#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"
#include <cstddef>

class BatteryProvider : public IStatsProvider, ReadProvider {
  private:
    std::string m_battery_path;

    std::string getBatteryPath();

    std::string getBatteryStatus();

    size_t getBatteryChargeNow();
    size_t getBatteryChargeFull();
    size_t getBatteryCurrentNow();

    size_t getBatteryPercentage(size_t chargeNow, size_t chargeFull);
    float getBatteryTimeRemaining(std::string status, size_t chargeNow,
                                  size_t chargeFull, size_t currentNow);

  public:
    BatteryProvider();
    void update(SystemStats &stats) override;
};
