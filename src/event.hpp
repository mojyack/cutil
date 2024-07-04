#pragma once
#include <atomic>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

// Single Waiter / Multiple Notifier
class Event {
  private:
    std::atomic_flag flag;

  public:
    auto wait() -> void {
        flag.wait(false);
        flag.clear();
    }

    auto notify() -> void {
        flag.test_and_set();
        flag.notify_all();
    }

    Event() = default;

    Event(const Event&) {}
};

#ifdef CUTIL_NS
}
#endif
