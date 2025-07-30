#include "providers/battery_provider.hpp"
#include "structs.hpp"

#include <cstddef>
#include <unistd.h>

std::string BatteryProvider::getBatteryStatus() {
    return readString(m_battery_path + "/status");
}

size_t BatteryProvider::getBatteryChargeNow() {
    return readValue<size_t>(m_battery_path + "/charge_now");
}

size_t BatteryProvider::getBatteryChargeFull() {
    return readValue<size_t>(m_battery_path + "/charge_full");
}

size_t BatteryProvider::getBatteryCurrentNow() {
    return readValue<size_t>(m_battery_path + "/current_now");
}

size_t BatteryProvider::getBatteryPercentage(size_t charge_now,
                                             size_t charge_full) {
    return static_cast<size_t>(100.0 * charge_now / charge_full);
}

float BatteryProvider::getBatteryTimeRemaining(std::string status,
                                               size_t charge_now,
                                               size_t charge_full,
                                               size_t current_now) {
    if (current_now > 0) {
        if (status == "Discharging") {
            return static_cast<float>(charge_now) / current_now * 60.0f;
        }
        if (status == "Charging") {
            return static_cast<float>(charge_full - charge_now) / current_now *
                   60.0f;
        }
    }
    return 0.0f;
}

std::string BatteryProvider::getBatteryPath() {
    const std::vector<std::string> battery_keywords = {"Battery"};

    std::string path =
        findDirByType("/sys/class/power_supply", battery_keywords);
    return path;
}

BatteryProvider::BatteryProvider() {
    m_battery_path = getBatteryPath();
}

void BatteryProvider::update(SystemStats &stats) {
    stats.battery.status = getBatteryStatus();
    stats.battery.charge_now = getBatteryChargeNow();
    stats.battery.charge_full = getBatteryChargeFull();
    stats.battery.current_now = getBatteryCurrentNow();
    stats.battery.percentage = getBatteryPercentage(stats.battery.charge_now,
                                                    stats.battery.charge_full);
    stats.battery.time_remaining = getBatteryTimeRemaining(
        stats.battery.status, stats.battery.charge_now,
        stats.battery.charge_full, stats.battery.current_now);
}

// struct BatteryStats {
//     std::string status;
//     size_t charge_now;
//     size_t charge_full;
//     size_t current_now;
//     size_t percentage;
//     float time_remaining;
// };
