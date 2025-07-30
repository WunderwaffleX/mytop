#include "providers/process_provider.hpp"
#include "structs.hpp"

#include <cstdint>
#include <fstream>
#include <regex>
#include <string>
#include <sys/statvfs.h>
#include <unistd.h>
#include <unordered_map>

void ProcessProvider::parseProcStat(const std::string &proc_path,
                                    ProcessInfo &proc) {
    std::ifstream stat_file(proc_path + "stat");
    std::string line;
    std::getline(stat_file, line);

    auto start = line.find('(');
    auto end = line.rfind(')');
    proc.pid = std::stoul(line.substr(0, start - 1));
    proc.name = line.substr(start + 1, end - start - 1);

    std::string after = line.substr(end + 2);
    std::istringstream iss(after);
    std::vector<std::string> fields;
    std::string field;
    while (iss >> field) {
        fields.push_back(field);
    }

    if (fields.size() >= 44) {
        proc.state = fields[0][0];
        proc.ppid = std::stoul(fields[1]);
        proc.priority = std::stoi(fields[15]);
        proc.nice = std::stoi(fields[16]);
        proc.threads = std::stoul(fields[17]);
        proc.virtual_memory = std::stoul(fields[20]);
        proc.resident_memory = std::stoul(fields[21]) * sysconf(_SC_PAGESIZE);

        unsigned long utime = std::stoul(fields[11]);
        unsigned long stime = std::stoul(fields[12]);
        proc.time = utime + stime;
    }
}

ProcessInfo ProcessProvider::getProcessInfo(std::string &proc_path) {
    ProcessInfo proc;
    parseProcStat(proc_path, proc);
    proc.user = readValue(proc_path + "loginuid", 0);
    proc.command = readString(proc_path + "cmdline", "");
    return proc;
}

ProcessProvider::ProcessProvider() {}

void ProcessProvider::update(SystemStats &stats) {
    namespace fs = std::filesystem;
    std::string path = "/proc/";
    const fs::path proc_dir(path);
    const std::regex proc_regex(R"(\d+)");

    stats.processes.clear();

    std::unordered_map<size_t, unsigned long> curProcTime;

    uint64_t delta_total = 0;
    if (!stats.cpu.time.empty() && !stats.cpu.prev_time.empty()) {
        uint64_t cpu_total = stats.cpu.time[0].total();
        uint64_t prev_cpu_total = stats.cpu.prev_time[0].total();

        if (cpu_total > prev_cpu_total) {
            delta_total = cpu_total - prev_cpu_total;
        }
    }

    long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);

    for (const auto &entry : fs::directory_iterator(proc_dir)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (std::regex_match(name, proc_regex)) {
                std::string proc_path = path + name + '/';

                ProcessInfo proc = getProcessInfo(proc_path);
                curProcTime[proc.pid] = proc.time;

                auto it = prevProcTime.find(proc.pid);
                unsigned long prev_proc_time =
                    (it != prevProcTime.end()) ? it->second : 0;

                if (delta_total > 0 && prev_proc_time != 0 &&
                    proc.time >= prev_proc_time) {
                    uint64_t delta_proc = proc.time - prev_proc_time;
                    proc.cpu_usage =
                        100.0f * delta_proc / delta_total * num_cpus;
                } else {
                    proc.cpu_usage = 0.0f;
                }

                stats.processes.push_back(proc);
            }
        }
    }

    prevProcTime = std::move(curProcTime);
}
