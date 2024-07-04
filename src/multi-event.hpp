#pragma once
#include "event.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

// Multiple Waiters / Single Notifier
class MultiEvent {
  private:
    std::atomic_int  total_waiters;
    Event            all_waiters_synced;
    std::atomic_flag notified;

  public:
    auto wait() -> void {
        total_waiters.fetch_add(1);

        notified.wait(false);

        if(total_waiters.fetch_sub(1) - 1 == 0) {
            all_waiters_synced.notify();
        }

        notified.wait(true);
    }

    auto notify(const bool finish = false) -> void {
        notified.test_and_set();
        notified.notify_all();

        all_waiters_synced.wait();

        notified.clear();
        notified.notify_all();
    }

    auto drain() -> void {
        while(total_waiters > 0) {
            notify();
        }
    }

    MultiEvent() = default;

    MultiEvent(const MultiEvent&) {}
};

#ifdef CUTIL_NS
}
#endif
