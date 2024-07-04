#pragma once
#include "event.hpp"

#ifdef CUTIL_WAITERS_EVENT_DEBUG
#include <iostream>
static auto cout_lock = std::mutex();
template <class... Args>
auto lock_print(Args... args) -> void {
    cout_lock.lock();
    (std::cout << ... << args) << std::endl;
    cout_lock.unlock();
}
#else
#define lock_print(...)
#endif

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

class WaitersEvent {
  private:
    std::atomic_int  total_waiters;
    std::atomic_int  synced_waiters;
    Event            all_waiters_synced;
    std::atomic_flag notified;

  public:
    auto join() -> void {
        total_waiters.fetch_add(1);
    }

    auto leave() -> void {
        if(synced_waiters == total_waiters.fetch_sub(1) - 1) {
            all_waiters_synced.notify();
        }
    }

    auto wait() -> void {
        lock_print("[wait] begin");
        if(synced_waiters.fetch_add(1) + 1 == total_waiters) {
            all_waiters_synced.notify();
        }

        lock_print("[wait] waiting");
        notified.wait(false);
        lock_print("[wait] waked");

        if(synced_waiters.fetch_sub(1) - 1 == 0) {
            all_waiters_synced.notify();
        }

        lock_print("[wait] exitting");
        notified.wait(true);
        lock_print("[wait] end");
    }

    auto notify() -> void {
        lock_print("[notify] begin");
        all_waiters_synced.wait();
        lock_print("[notify] all waiters arrived");

        if(total_waiters == 0) {
            // all waiters leaved
            return;
        }

        notified.test_and_set();
        notified.notify_all();

        lock_print("[notify] waiting for exit");
        all_waiters_synced.wait();
        lock_print("[notify] all waiters exitted");

        notified.clear();
        notified.notify_all();
        lock_print("[notify] end");
    }

    auto drain() -> void {
        while(total_waiters > 0) {
            notify();
        }
    }

    WaitersEvent() = default;

    WaitersEvent(const WaitersEvent&) {}
};

#ifdef CUTIL_NS
}
#endif
