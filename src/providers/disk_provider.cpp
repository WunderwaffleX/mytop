#include "providers/disk_provider.hpp"
#include "structs.hpp"

#include <sys/statvfs.h>

void DiskProvider::parseMtabLine(const std::string &line, DiskStats &t) {
    std::istringstream iss(line);
    iss >> t.partition >> t.name >> t.filesystem;
}

void DiskProvider::parseMtab(std::vector<DiskStats> &stats) {
    std::ifstream mtab_file("/etc/mtab");
    std::string line;
    while (std::getline(mtab_file, line)) {
        if (line.starts_with("/dev/")) {
            DiskStats t{};
            parseMtabLine(line, t);
            stats.push_back(t);
        }
    }
}

void DiskProvider::getDiskInfo(DiskStats &disk) {
    struct statvfs buf;
    if (statvfs(disk.name.c_str(), &buf) != 0)
        return;
    uint64_t total = buf.f_blocks * buf.f_frsize;
    uint64_t avail = buf.f_bavail * buf.f_frsize;
    uint64_t used = total - avail;

    float usage = total == 0 ? 0.0f : (float)used / total * 100.0f;

    disk.total_space = total;
    disk.used_space = used;
    disk.usage_percent = usage;
}

DiskProvider::DiskProvider() {}

void DiskProvider::update(SystemStats &stats) {
    stats.disk.clear();

    parseMtab(stats.disk);
    for (DiskStats &disk : stats.disk) {
        getDiskInfo(disk);
    }
}

// struct DiskStats {
//     std::string name;
//     std::string partition;
//     std::string filesystem;
//     uint64_t total_space;
//     uint64_t used_space;
//     float usage_percent;
// };
