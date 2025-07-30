#pragma once

#include "formatter.hpp"
#include "structs.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

class UI {
  private:
    AppState &state_;
    Formatter &formatter_;

    ftxui::ScreenInteractive screen_;
    ftxui::Component renderer_;

    int scroll_offset_ = 0;
    size_t current_tab_ = 2;
    int selected_index_ = 0;
    int visible_rows_ = 20;

    bool change_proc_ = false;
    ProcessInfo selected_proc_;

    bool show_process_menu_ = false;

    SortOrder sort_order_ = SortOrder::MEMORY;
    bool sort_reversed_ = false;

    std::function<std::vector<int>(int, int)> cpu_history_func;
    std::function<std::vector<int>(int, int)> memory_history_func;

    std::vector<std::string> process_strings_;

    ftxui::Element render_ui(const SystemStats &stats);

    ftxui::Element render_stats_tab(const SystemStats &stats);

    ftxui::Element render_cpu(const CPUStats &cpu);
    ftxui::Element render_memory(const MemoryStats &mem);
    ftxui::Element render_battery(const BatteryStats &bat);
    ftxui::Element render_disks(const std::vector<DiskStats> &disks);
    ftxui::Element render_gpu(const std::vector<GPUStats> &gpus);

    ftxui::Element render_process_modal(const ProcessInfo &proc);

    ftxui::Element render_process_tab(std::vector<ProcessInfo> processes);
    ftxui::Element render_help_tab();

  public:
    explicit UI(AppState &state, Formatter &formatter);

    void run();
    void notify_refresh();
};
