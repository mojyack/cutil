#pragma once
#include <atomic>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

namespace {
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
} // namespace

#ifdef CUTIL_NS
}
#endif
