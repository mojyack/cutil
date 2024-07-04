#include <random>
#include <thread>

// #define CUTIL_WAITERS_EVENT_DEBUG
#include "print.hpp"
#include "waiters-event.hpp"

auto engine = std::mt19937((std::random_device())());

#ifndef CUTIL_WAITERS_EVENT_DEBUG
#undef lock_print
#include "lock-print.hpp"
#endif

auto main() -> int {
    constexpr auto num_iterates = 100;
    constexpr auto num_threads  = 100;

    auto running = true;
    auto threads = std::array<std::thread, num_threads>();

    auto waiters_event = WaitersEvent();
    auto target        = std::atomic_int(-1);
    auto target_event  = Event();
    auto ready         = std::atomic_int(0);
    for(auto i = 0u; i < threads.size(); i += 1) {
        threads[i] = std::thread([&, i]() {
            while(running) {
                waiters_event.join();
                ready.fetch_add(1);
                while(running) {
                    waiters_event.wait();
                    // lock_print2("thread ", i, " waked up");
                    // std::this_thread::sleep_for(std::chrono::milliseconds(uint8_t(engine() / 255.0 * 100)));
                    if(auto expected = int(i); target.compare_exchange_strong(expected, -1)) {
                        lock_print("thread ", i, " specified");
                        target_event.notify();
                        break;
                    } else {
                        // lock_print("not me, target was ", expected);
                    }
                }
                waiters_event.leave();
            }
        });
    }

    while(ready != num_threads) {
        print("waiting for threads...");
    }

    for(auto i = 0; i < num_iterates; i += 1) {
        lock_print("==== iteration ", i, " ====");
        for(auto t = 0; t < num_threads; t += 1) {
            lock_print("==== thread ", t, " ====");
            target = t;
            waiters_event.notify();
            target_event.wait();
            // to see if threads are really sleeping
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    running = false;
    waiters_event.drain();
    for(auto i = 0u; i < threads.size(); i += 1) {
        threads[i].join();
    }

    print("pass");

    return 0;
}
