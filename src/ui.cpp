#include "ui.hpp"
#include "formatter.hpp"
#include "structs.hpp"
#include <fmt/core.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/string.hpp>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <vector>

using namespace ftxui;

Element UI::render_cpu(const CPUStats &cpu) {

    auto summary = vbox({
        text(fmt::format("Name: {}", cpu.name)),
        text(fmt::format("Usage: {:.1f}%", cpu.usage_percent)),
        text(fmt::format("Temperature: {} °C", cpu.temperature)),
        text(fmt::format("Frequency: {} MHz", cpu.frequency_mhz)),
        graph(cpu_history_func) | color(Color::BlueViolet),
    });

    size_t half = (cpu.cores.size() + 1) / 2;
    Elements left_column, right_column;

    for (size_t i = 0; i < cpu.cores.size(); ++i) {
        const auto &core = cpu.cores[i];
        auto line = text(fmt::format("Core {}: {:.1f}%, {} MHz", core.coreid,
                                     core.usage_percent, core.frequency_mhz));
        (i < half ? left_column : right_column).push_back(line);
    }

    auto core_info = gridbox({{
        vbox(std::move(left_column)),
        separator(),
        vbox(std::move(right_column)),
    }});

    return window(text(" CPU "), gridbox({{summary, separator(), core_info}}));
}

Element UI::render_memory(const MemoryStats &mem) {
    constexpr int max_graph_width = 50;

    return window(
        text(" Memory "),
        hbox({
            vbox({text(fmt::format("Total: {:.1f} Gb", mem.total_gb)),
                  text(fmt::format("Used: {:.1f} Gb ({:.1f}%)", mem.used_gb,
                                   mem.used_percent)),
                  text(fmt::format("Free: {:.1f} Gb", mem.free_gb)),
                  text(fmt::format("Available: {:.1f} Gb", mem.available_gb)),
                  text(fmt::format("Cached: {:.1f} Gb", mem.cached_gb)),
                  text(fmt::format("Swap Total: {:.1f} Gb", mem.swap_total_gb)),
                  text(fmt::format("Swap Used: {:.1f} Gb", mem.swap_used_gb))}),
            graph(memory_history_func) | size(WIDTH, EQUAL, max_graph_width),
        }));
}

Element UI::render_battery(const BatteryStats &bat) {
    return window(text(" Battery "),
                  vbox({
                      text(fmt::format("{}: {}% ({})", bat.status,
                                       bat.percentage, bat.time_remaining)),
                  }));
}

Element UI::render_disks(const std::vector<DiskStats> &disks) {
    Elements disk_boxes;

    for (size_t i = 0; i < disks.size(); ++i) {
        const auto &d = disks[i];
        disk_boxes.push_back(flex(vbox({
            text(fmt::format("Name: {}", d.name)),
            text(fmt::format("Partition: {}", d.partition)),
            text(fmt::format("Filesystem: {}", d.filesystem)),
            text(fmt::format("Total: {:.1f} GB", d.total_space_gb)),
            text(fmt::format("Used: {:.1f} GB ({:.1f}%)", d.used_space_gb,
                             d.usage_percent)),
        })));

        if (i + 1 < disks.size())
            disk_boxes.push_back(separator());
    }

    return window(text(" Disks "), hbox(std::move(disk_boxes)));
}

Element UI::render_gpu(const std::vector<GPUStats> &gpus) {
    Elements gpu_boxes;

    for (size_t i = 0; i < gpus.size(); ++i) {
        const auto &gpu = gpus[i];
        gpu_boxes.push_back(
            vbox({
                text(fmt::format("Name: {}", gpu.name)),
                text(fmt::format("Usage: {:.1f}%", gpu.usage_percent)),
                text(fmt::format("Temperature: {} °C", gpu.temperature)),
                text(fmt::format("Memory: {} / {} Mb", gpu.memory_used,
                                 gpu.memory_total)),
            }) |
            flex);

        if (i + 1 < gpus.size())
            gpu_boxes.push_back(separator());
    }

    return window(text(fmt::format(" GPU {}", gpus.size())),
                  vbox(std::move(gpu_boxes)));
}

std::function<std::vector<int>(int, int)>
make_history_func(std::vector<int> &history) {
    return [&history](int width, int height) -> std::vector<int> {
        std::vector<int> output;
        if (history.empty() || width <= 0 || height <= 0)
            return output;

        const size_t max_history_size = 200;
        if (history.size() > max_history_size) {
            history.erase(history.begin(), history.end() - max_history_size);
        }

        int start_index = std::max(0, static_cast<int>(history.size()) - width);
        auto slice =
            std::vector<int>(history.begin() + start_index, history.end());

        if (slice.empty())
            return std::vector<int>(width, 0);

        int max_val = *std::max_element(slice.begin(), slice.end());
        max_val = std::max(1, max_val);

        output.reserve(slice.size());
        for (int val : slice) {
            output.push_back(val * height / max_val);
        }

        while (static_cast<int>(output.size()) < width) {
            output.insert(output.begin(), 0);
        }

        return output;
    };
}

Element UI::render_stats_tab(const SystemStats &stats) {
    return vbox({
        render_battery(stats.battery),
        render_cpu(stats.cpu),
        render_gpu(stats.gpu),
        render_memory(stats.memory),
        render_disks(stats.disk),
    });
}

Element UI::render_process_modal(const ProcessInfo &proc) {
    auto title = text(fmt::format(" Process {} ", proc.pid)) | bold |
                 color(Color::White);
    auto body = vbox({
        text(fmt::format("Name: {}", proc.name)),
        text(fmt::format("PID: {}", proc.pid)),
        text(fmt::format("User: {}", proc.user)),
        text(fmt::format("Priority: {}", proc.priority)),
        text(fmt::format("Threads: {}", proc.threads)),
        // text(fmt::format("CPU: {:.1f}%", proc.cpu_usage)),
        // text(fmt::format("Memory: {:.1f} MB",
        //                  proc.resident_memory / (1024.0 * 1024.0))),
        // text(fmt::format("State: {}", proc.state)),
        // text(fmt::format("Command: {}", proc.command)),
        separator(),
        text("Press S - SIGSTOP | T - SIGTERM | K - SIGKILL"),
        text("Press ESC to return"),
    });

    return center(hbox({
        filler(),
        vbox({
            title,
            body,
        }) | borderDouble,
        filler(),
    }));
}

Element UI::render_process_tab(std::vector<ProcessInfo> processes) {
    Elements table;

    auto truncate = [](const std::string &str, size_t max_len) {
        if (str.size() <= max_len)
            return str;
        return str.substr(0, max_len - 3) + "...";
    };

    std::string sort_indicator_;
    std::vector<size_t> *active_index;

    switch (sort_order_) {
    case SortOrder::PID:
        sort_indicator_ = "↓PID";
        active_index = &state_.idx_pid;
        break;
    case SortOrder::NAME:
        sort_indicator_ = "↓Name";
        active_index = &state_.idx_name;
        break;
    case SortOrder::CPU:
        sort_indicator_ = "↓CPU%";
        active_index = &state_.idx_cpu;
        break;
    case SortOrder::MEMORY:
        sort_indicator_ = "↓Memory";
        active_index = &state_.idx_memory;
        break;
    case SortOrder::STATE:
        sort_indicator_ = "↓State";
        active_index = &state_.idx_state;
        break;
    }

    table.push_back(hbox({
        text("PID") | bold | size(WIDTH, EQUAL, 6),
        text("PPID") | bold | size(WIDTH, EQUAL, 6),
        text("Name") | bold | size(WIDTH, EQUAL, 20),
        text("CPU%") | bold | size(WIDTH, EQUAL, 6),
        text("Mem") | bold | size(WIDTH, EQUAL, 16),
        text("State") | bold | size(WIDTH, EQUAL, 6),
        text("Command") | bold | flex,
    }));

    if (selected_index_ < 0)
        selected_index_ = 0;
    if (selected_index_ >= static_cast<int>(processes.size()))
        selected_index_ = processes.size() - 1;

    int start = std::max(0, scroll_offset_);
    int end =
        std::min(start + visible_rows_, static_cast<int>(processes.size()));

    for (int i = start; i < end; ++i) {
        size_t index_in_processes;
        if (sort_reversed_) {
            index_in_processes = (*active_index)[active_index->size() - 1 - i];
        } else {
            index_in_processes = (*active_index)[i];
        }

        const auto &proc = processes[index_in_processes];
        auto row = hbox({
            text(fmt::format("{}", proc.pid)) | size(WIDTH, EQUAL, 6),
            text(fmt::format("{}", proc.ppid)) | size(WIDTH, EQUAL, 6),
            text(proc.name) | size(WIDTH, EQUAL, 20),
            text(fmt::format("{:.1f}", proc.cpu_usage)) | size(WIDTH, EQUAL, 6),
            text(fmt::format("{:.1f} Mb",
                             proc.resident_memory / (1024.0 * 1024.0))) |
                size(WIDTH, EQUAL, 16),
            text(std::string(1, proc.state)) | size(WIDTH, EQUAL, 6),
            text(truncate(proc.command, 100)) | flex,
        });

        if (static_cast<int>(i) == selected_index_) {
            row = row | inverted;
            if (change_proc_) {
                change_proc_ = false;
                selected_proc_ = proc;
            }
        }

        table.push_back(row);
    }
    return window(text(fmt::format(" Processes ({})", sort_indicator_)),
                  vbox({vbox(std::move(table)) | frame | vscroll_indicator |
                        size(HEIGHT, LESS_THAN, visible_rows_)}));
}

Element render_help_tab() {
    int angle = 100.f;
    float start = 0.f;
    float end = 1.f;

    auto background = text("HELP! XD") | center |
                      bgcolor(LinearGradient()
                                  .Angle(angle)
                                  .Stop(Color::Blue, start)
                                  .Stop(Color::Red, end));

    return vbox({
               background | flex,
           }) |
           flex;
}

Element UI::render_ui(const SystemStats &stats) {
    if (current_tab_ == 1) {

        return render_process_tab(stats.processes);
    }
    return render_stats_tab(stats);
}

UI::UI(AppState &state, Formatter &formatter)
    : state_(state), screen_(ScreenInteractive::Fullscreen()),
      formatter_(formatter) {

    cpu_history_func = make_history_func(state_.cpu_history);
    memory_history_func = make_history_func(state_.memory_history);

    Component base_component = Renderer([this] {
        std::lock_guard<std::mutex> lock(state_.mutex);
        const auto &formatted = state_.stats;
        return render_ui(formatted);
    });

    Component modal_component =
        Renderer([this] { return render_process_modal(selected_proc_); });

    base_component |= Modal(modal_component, &show_process_menu_);

    renderer_ = CatchEvent(base_component, [this, &state](const Event &event) {
        if (event == Event::Character('q') ||
            event == Event::Character('\003')) {
            screen_.Exit();
            return true;
        }

        if (event == Event::Tab || event == Event::Character('\t')) {
            visible_rows_ = screen_.dimy() - 2;
            current_tab_ = (current_tab_ + 1) % 2;
            show_process_menu_ = false;
            return true;
        }

        if (current_tab_ == 1) {
            std::lock_guard<std::mutex> lock(state_.mutex);
            const auto &proc_count = state_.stats.processes.size();

            if (event == Event::ArrowUp) {
                if (selected_index_ > 0) {
                    --selected_index_;
                    if (selected_index_ < scroll_offset_ + 4) {
                        scroll_offset_ = std::max(0, selected_index_ - 4);
                    }
                }
                show_process_menu_ = false;
                return true;
            }
            if (event == Event::ArrowDown) {
                if (selected_index_ + 1 < static_cast<int>(proc_count)) {
                    ++selected_index_;
                    if (selected_index_ > scroll_offset_ + visible_rows_ - 5) {
                        scroll_offset_ = std::min(
                            static_cast<int>(proc_count) - visible_rows_,
                            selected_index_ - visible_rows_ + 5);
                    }
                }
                show_process_menu_ = false;
                return true;
            }

            if (event == Event::ArrowLeft || event == Event::ArrowRight) {
                if (event == Event::ArrowRight) {
                    sort_order_ = static_cast<SortOrder>(
                        (static_cast<int>(sort_order_) + 1) % 5);
                } else {
                    sort_order_ = static_cast<SortOrder>(
                        (static_cast<int>(sort_order_) + 4) % 5);
                }

                selected_index_ = 0;
                scroll_offset_ = 0;

                return true;
            }
            if (event == Event::Character('r')) {
                sort_reversed_ = !sort_reversed_;
                return true;
            }
            if (event == Event::Return) {
                change_proc_ = true;
                show_process_menu_ = true;
                return true;
            }
            // TODO: make search 'f'
            if (show_process_menu_ && event == Event::Escape) {
                show_process_menu_ = false;
                return true;
            }
        }

        if (show_process_menu_) {
            if (event == Event::Character('s')) {
                kill(selected_proc_.pid, SIGSTOP);
                show_process_menu_ = false;
                return true;
            }
            if (event == Event::Character('t')) {
                kill(selected_proc_.pid, SIGTERM);
                show_process_menu_ = false;
                return true;
            }
            if (event == Event::Character('k')) {
                kill(selected_proc_.pid, SIGKILL);
                show_process_menu_ = false;
                return true;
            }
        }

        return false;
    });
}

void UI::notify_refresh() {
    screen_.PostEvent(Event::Custom);
}

void UI::run() {
    screen_.Loop(renderer_);
}
