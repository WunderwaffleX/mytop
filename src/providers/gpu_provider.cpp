#include "providers/gpu_provider.hpp"
#include "nvml.h"
#include "structs.hpp"
#include <iostream>
#include <regex>

std::string get_driver_name_for_card(const std::string &card_path) {
    namespace fs = std::filesystem;

    try {
        fs::path driver_symlink =
            fs::canonical(fs::path(card_path) / "device" / "driver");
        return driver_symlink.filename().string();
    } catch (const fs::filesystem_error &e) {
        return "unknown_driver";
    }
}

GpuProvider::GpuProvider() {
    namespace fs = std::filesystem;

    const fs::path gpu_path("/sys/class/drm/");
    const std::regex gpu_regex(R"(card\d+)");

    for (const auto &entry : fs::directory_iterator(gpu_path)) {
        if (!fs::is_symlink(entry))
            continue;

        std::string name = entry.path().filename().string();
        if (!std::regex_match(name, gpu_regex))
            continue;

        fs::path real_path = fs::canonical(entry.path());
        fs::path vendor_path = real_path.parent_path().parent_path() / "vendor";

        std::string vendor_id = readString(vendor_path);

        DetectedGPUInfo gpu_info{
            .vendor_id = vendor_id,
            .device_path = fs::canonical(entry.path()).string(),
            .card_name = name,
        };

        m_detected_gpus.push_back(gpu_info);
    }

    nvmlInit_v2();
}

void GpuProvider::update(SystemStats &stats) {
    namespace fs = std::filesystem;

    stats.gpu.clear();

    unsigned int nvml_index = 0;

    for (const auto &info : m_detected_gpus) {
        GPUStats stat;
        // stat.name = info.card_name;

        if (info.vendor_id == "0x10de") { // NVIDIA
            nvmlDevice_t device;
            if (nvmlDeviceGetHandleByIndex(nvml_index++, &device) ==
                NVML_SUCCESS) {
                nvmlMemory_t mem_info;
                nvmlDeviceGetMemoryInfo(device, &mem_info);

                char name[NVML_DEVICE_NAME_BUFFER_SIZE];
                if (nvmlDeviceGetName(device, name,
                                      NVML_DEVICE_NAME_BUFFER_SIZE) ==
                    NVML_SUCCESS) {
                    stat.name = std::string(name);
                }

                stat.memory_total = mem_info.total / 1024 / 1024;
                stat.memory_used = mem_info.used / 1024 / 1024;

                unsigned int temp;
                nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
                stat.temperature = temp;

                nvmlUtilization_t util;
                nvmlDeviceGetUtilizationRates(device, &util);
                stat.usage_percent = util.gpu;
            }
        } else if (info.vendor_id == "0x8086") { // Intel
            std::filesystem::path real_path =
                fs::canonical(info.device_path); // .../drm/cardX
            std::filesystem::path pci_device_path =
                real_path.parent_path()
                    .parent_path(); // .../pciXXXX/XXXX:XX:XX.X

            std::string driver_name =
                get_driver_name_for_card(info.device_path);
            stat.name = info.card_name + " (" + driver_name + ")";

            std::filesystem::path hwmon_base = pci_device_path / "hwmon";
            if (fs::exists(hwmon_base)) {
                for (const auto &hwmon :
                     std::filesystem::directory_iterator(hwmon_base)) {
                    std::ifstream fin(hwmon.path() / "temp1_input");
                    if (fin.is_open()) {
                        int temp_raw;
                        fin >> temp_raw;
                        stat.temperature = temp_raw / 1000;
                        break;
                    }
                }
            }

            stat.usage_percent = 0.0;
            stat.memory_total = 0;
            stat.memory_used = 0;
        }

        stats.gpu.push_back(stat);
    }
}
