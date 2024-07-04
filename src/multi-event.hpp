#pragma once
#include <atomic>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

// Multiple Waiter / Single Notifier
class MultiEvent {
  private:
    std::atomic_int  waiters;
    std::atomic_flag waking;
    std::atomic_flag notified;
    std::atomic_flag waiters_complete;

  public:
    auto wait() -> void {
        waking.wait(true);

        waiters.fetch_add(1);
        notified.wait(false);
        if(waiters.fetch_sub(1) == 1) {
            waiters_complete.test_and_set();
            waiters_complete.notify_one();
        }
    }

    auto notify(const bool finish = false) -> void {
        waking.test_and_set();

        notified.test_and_set();
        notified.notify_all();
        waiters_complete.wait(false);
        if(!finish) {
            notified.clear();
            waiters_complete.clear();
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
