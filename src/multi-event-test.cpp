#include <thread>

#include "multi-event.hpp"
#include "print.hpp"

auto main() -> int {
    constexpr auto num_iterates = 100;
    constexpr auto num_threads  = 100;

    auto running     = true;
    auto count       = std::atomic_int(0);
    auto multi_event = MultiEvent();
    auto threads     = std::array<std::thread, num_threads>();
    for(auto i = 0u; i < threads.size(); i += 1) {
        threads[i] = std::thread([&running, &count, &multi_event]() {
            while(running) {
                count.fetch_add(1);
                multi_event.wait();
            }
        });
    }

    for(auto i = 0; i < num_iterates; i += 1) {
        multi_event.notify();
    }

    print("ideal: ", num_iterates * num_threads, " result: ", int(count));

    running = false;
    multi_event.drain();
    for(auto i = 0u; i < threads.size(); i += 1) {
        threads[i].join();
    }

    print("pass");

    return 0;
}
