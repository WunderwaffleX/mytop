#include "app.hpp"
#include "formatter.hpp"
#include "system_monitor.hpp"
#include "ui.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>

void App::run() {
    AppState state;
    SystemMonitor monitor;
    Formatter formatter(state);
    UI ui(state, formatter);

    std::atomic<bool> running = true;
    std::mutex mtx;
    std::condition_variable cv;

    std::thread updater([&] {
        std::unique_lock<std::mutex> lock(mtx);
        while (running) {
            lock.unlock();
            // TODO:
            // start time

            auto new_stats = monitor.collect();

            {
                std::lock_guard<std::mutex> state_lock(state.mutex);

                state.stats = new_stats;

                state.cpu_history.push_back(
                    static_cast<int>(new_stats.cpu.usage_percent));
                state.memory_history.push_back(
                    static_cast<int>(new_stats.memory.used_percent));

                formatter.format(state.stats);
            }

            ui.notify_refresh();

            lock.lock();
            // end time >= start time + 1000
            cv.wait_for(lock, std::chrono::milliseconds(1000),
                        [&] { return !running; });
        }
    });
    ui.run();
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
    }
    cv.notify_all();
    updater.join();
}
