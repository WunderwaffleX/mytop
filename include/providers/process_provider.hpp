#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"
#include "structs.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_map>

struct ProcInfo {
    char state;
    size_t ppid, pgrp, session, tty_nr, tpgid;
    size_t flags;
    size_t minflt, cminflt, majflt, cmajflt;
    size_t utime, stime, cutime, cstime;
    long priority, nice;
    size_t num_threads, starttime;
    size_t vsize, rss;
};

class ProcessProvider : public IStatsProvider, ReadProvider {
  private:
    std::unordered_map<size_t, unsigned long> prevProcTime;

    void parseProcStat(const std::string &proc_path, ProcessInfo &proc);
    ProcessInfo getProcessInfo(std::string &proc_path);

  public:
    ProcessProvider();
    void update(SystemStats &stats) override;
};
