#include "providers/cpu_provider.hpp"
#include "structs.hpp"

#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>

CpuTime CpuProvider::parseCpuTimeLine(const std::string &line) {
    std::istringstream iss(line);
    std::string cpu;
    CpuTime t{};
    iss >> cpu >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq >>
        t.softirq >> t.steal;
    return t;
}

float CpuProvider::getCpuUsage() {
    std::ifstream stat1("/proc/stat");
    std::string line1;
    std::getline(stat1, line1);
    CpuTime t1 = parseCpuTimeLine(line1);
    stat1.close();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ifstream stat2("/proc/stat");
    std::string line2;
    std::getline(stat2, line2);
    CpuTime t2 = parseCpuTimeLine(line2);
    stat2.close();

    uint64_t totalDelta = t2.total() - t1.total();
    uint64_t idleDelta = t2.idleTime() - t1.idleTime();

    if (totalDelta == 0)
        return 0.0f;

    return 100.0f * (totalDelta - idleDelta) / totalDelta;
}

size_t CpuProvider::getCpuTemperature() {
    if (thermalPath.empty())
        return 0;

    std::ifstream temp_file(thermalPath);
    if (!temp_file.is_open())
        return 0;

    int temp_milli = 0;
    temp_file >> temp_milli;

    return temp_milli / 1000;
}

float CpuProvider::getCpuFreq() {
    return readValue<float>(freqPath, 0.0f);
}

std::string CpuProvider::getThermalPath() {
    const std::vector<std::string> cpu_zone_keywords = {"x86_pkg_temp"};

    std::string path = findDirByType("/sys/class/thermal", cpu_zone_keywords);
    if (path.empty()) {
        return "";
    }
    return path + "/temp";
}

std::string CpuProvider::getCpuNameFromSystem() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            return line.substr(line.find(':') + 2);
        }
    }
    return "Unknown CPU";
}

CpuProvider::CpuProvider() {
    cpuName = getCpuNameFromSystem();
    thermalPath = getThermalPath();
    freqPath = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
}

void CpuProvider::update(SystemStats &stats) {
    stats.cpu.name = cpuName;
    stats.cpu.usage_percent = getCpuUsage();
    stats.cpu.temperature = getCpuTemperature();
    stats.cpu.frequency_mhz = getCpuFreq();
    stats.cpu.cores.clear();
}
