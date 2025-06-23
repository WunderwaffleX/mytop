#pragma once

#include "iprovider.hpp"
#include "read_provider.hpp"
#include "structs.hpp"

class DiskProvider : public IStatsProvider, ReadProvider {
  private:
    void parseMtabLine(const std::string &line, DiskStats &t);
    void parseMtab(std::vector<DiskStats> &stats);
    void getDiskInfo(DiskStats &disk);

  public:
    DiskProvider();
    void update(SystemStats &stats) override;
};
