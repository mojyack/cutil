#pragma once
#include <atomic>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

class Event {
  private:
    std::atomic_flag flag;

  public:
    auto clear() -> void {
        flag.clear();
    }

    auto wait() -> void {
        flag.wait(false);
    }

    auto wakeup() -> void {
        flag.test_and_set();
        flag.notify_all();
    }

    Event() = default;

    Event(const Event&) {}
};

#ifdef CUTIL_NS
}
#endif
