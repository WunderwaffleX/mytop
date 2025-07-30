#include "providers/cpu_provider.hpp"
#include "structs.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

CpuTime CpuProvider::parseCpuTimeLine(const std::string &line) {
    std::istringstream iss(line);
    std::string cpu;
    CpuTime t{};
    iss >> cpu >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq >>
        t.softirq >> t.steal;
    return t;
}

std::vector<CpuTime> CpuProvider::getCpuTimeSnapshot() {
    std::ifstream stat_file("/proc/stat");
    std::vector<CpuTime> times;
    std::string line;

    while (std::getline(stat_file, line)) {
        if (line.rfind("cpu", 0) == 0) {
            times.push_back(parseCpuTimeLine(line));
        } else {
            break;
        }
    }

    return times;
}

float CpuProvider::getCpuUsage(const CpuTime &t1, const CpuTime &t2) {
    uint64_t totalDelta = t2.total() - t1.total();
    uint64_t idleDelta = t2.idleTime() - t1.idleTime();

    if (totalDelta == 0)
        return 0.0f;

    return 100.0f * (totalDelta - idleDelta) / totalDelta;
}

size_t CpuProvider::getCpuTemperature() {
    if (m_thermalPath.empty())
        return 0;

    std::ifstream temp_file(m_thermalPath);
    if (!temp_file.is_open())
        return 0;

    int temp_milli = 0;
    temp_file >> temp_milli;

    return temp_milli / 1000;
}

std::vector<CoreStats> CpuProvider::getCoreStats(std::vector<CpuTime> t1,
                                                 std::vector<CpuTime> t2) {

    if (t1.size() != m_coresAmount + 1 || t2.size() != m_coresAmount + 1) {
        return {};
    }

    namespace fs = std::filesystem;
    std::string path = "/sys/devices/system/cpu/";
    const fs::path cpu_path(path);
    std::vector<CoreStats> core_stats;

    for (int i = 0; i < m_coresAmount; i++) {
        std::string freq_path =
            path + "cpu" + std::to_string(i) + "/cpufreq/scaling_cur_freq";
        float freq = readValue<float>(freq_path, 0.0f);

        CoreStats stats{};

        stats.coreid = i;

        stats.frequency_mhz = freq;

        stats.usage_percent = getCpuUsage(t1[i + 1], t2[i + 1]);

        core_stats.push_back(stats);
    }
    return core_stats;
}

float CpuProvider::getCpuFreq() {
    return readValue<float>(m_freqPath, 0.0f);
}

int CpuProvider::getCoresAmount() {
    namespace fs = std::filesystem;
    const fs::path cpu_path("/sys/devices/system/cpu/");
    int cnt = 0;
    const std::regex cpu_regex(R"(cpu\d+)");

    for (const auto &entry : fs::directory_iterator(cpu_path)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (std::regex_match(name, cpu_regex)) {
                ++cnt;
            }
        }
    }
    return cnt;
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
    m_cpuName = getCpuNameFromSystem();
    m_coresAmount = getCoresAmount();
    m_thermalPath = getThermalPath();
    m_freqPath = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
}

void CpuProvider::update(SystemStats &stats) {
    std::vector<CpuTime> times1 = getCpuTimeSnapshot();

    if (times1.empty())
        return;

    stats.cpu.prev_time = stats.cpu.time;

    stats.cpu.time = times1;

    stats.cpu.name = m_cpuName;
    stats.cpu.temperature = getCpuTemperature();
    stats.cpu.frequency_mhz = getCpuFreq();

    if (!stats.cpu.prev_time.empty()) {
        stats.cpu.usage_percent =
            getCpuUsage(stats.cpu.prev_time[0], stats.cpu.time[0]);
        stats.cpu.cores = getCoreStats(stats.cpu.prev_time, stats.cpu.time);
    } else {
        stats.cpu.usage_percent = 0.0f;
        stats.cpu.cores.clear();
    }
}
