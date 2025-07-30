#pragma once

#include "formatter.hpp"
#include "structs.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

class UI {
  private:
    AppState &m_state;
    Formatter &m_formatter;

    ftxui::ScreenInteractive m_screen;
    ftxui::Component m_renderer;

    int m_scroll_offset = 0;
    size_t m_current_tab = 2;
    int m_selected_index = 0;
    int m_visible_rows = 20;

    bool m_change_proc = false;
    ProcessInfo m_selected_proc;

    bool m_show_process_menu = false;

    SortOrder m_sort_order = SortOrder::MEMORY;
    bool m_sort_reversed = false;

    std::function<std::vector<int>(int, int)> cpu_history_func;
    std::function<std::vector<int>(int, int)> memory_history_func;

    std::vector<std::string> m_process_strings;

    ftxui::Element renderUI(const SystemStats &stats);

    ftxui::Element renderStatsTab(const SystemStats &stats);

    ftxui::Element renderCPU(const CPUStats &cpu);
    ftxui::Element renderMemory(const MemoryStats &mem);
    ftxui::Element renderBattery(const BatteryStats &bat);
    ftxui::Element renderDisks(const std::vector<DiskStats> &disks);
    ftxui::Element renderGPU(const std::vector<GPUStats> &gpus);

    ftxui::Element renderProcessModal(const ProcessInfo &proc);

    ftxui::Element renderProcessTab(std::vector<ProcessInfo> processes);
    ftxui::Element renderHelpTab();

  public:
    explicit UI(AppState &state, Formatter &formatter);

    void run();
    void notifyRefresh();
};
