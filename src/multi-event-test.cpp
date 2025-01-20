#include <array>
#include <print>
#include <thread>

#include "multi-event.hpp"

auto main() -> int {
    constexpr auto num_iterates = 100;
    constexpr auto num_threads  = 100;

    auto running     = true;
    auto count       = std::atomic_int(0);
    auto exitted     = std::atomic_int(0);
    auto multi_event = MultiEvent();
    auto threads     = std::array<std::thread, num_threads>();
    for(auto i = 0u; i < threads.size(); i += 1) {
        threads[i] = std::thread([&]() {
            while(running) {
                count.fetch_add(1);
                multi_event.wait();
            }
            exitted.fetch_add(1);
        });
    }

    for(auto i = 0; i < num_iterates; i += 1) {
        multi_event.notify();
    }

    std::println("ideal={} result={}", num_iterates * num_threads, int(count));

    running = false;
    while(exitted != num_threads) {
        multi_event.notify_unblock();
    }
    for(auto i = 0u; i < threads.size(); i += 1) {
        threads[i].join();
    }

    std::println("pass");

    return 0;
}
