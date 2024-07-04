#pragma once
#include "event.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

// Multiple Waiter / Single Notifier
class MultiEvent {
  private:
    std::atomic_int  waiters;
    std::atomic_flag waking;
    std::atomic_flag notified;
    Event            waiters_complete;

  public:
    auto wait() -> void {
        waking.wait(true);

        waiters.fetch_add(1);
        notified.wait(false);
        if(waiters.fetch_sub(1) == 1) {
            waiters_complete.notify();
        }
    }

    auto notify(const bool finish = false) -> void {
        waking.test_and_set();

        notified.test_and_set();
        notified.notify_all();
        waiters_complete.wait();
        if(!finish) {
            notified.clear();
        }

        waking.clear();
        waking.notify_all();
    }

    MultiEvent() = default;

    MultiEvent(const MultiEvent&) {}
};

#ifdef CUTIL_NS
}
#endif
