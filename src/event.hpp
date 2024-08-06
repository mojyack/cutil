#pragma once
#include <atomic>

namespace CUTIL_NS {
// Single Waiter / Multiple Notifiers
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
        flag.notify_one();
    }

    Event() = default;

    Event(const Event&) {}
};
} // namespace CUTIL_NS
