#pragma once
#include <condition_variable>

#include "critical.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

namespace {
class TimerEvent {
  private:
    std::condition_variable condv;
    Critical<bool>          waked;

  public:
    auto wait() -> void {
        waked.access().second = false;
        auto lock             = std::unique_lock<std::mutex>(waked.get_raw_mutex());
        condv.wait(lock, [this]() { return waked.assume_locked(); });
    }

    // returns: true => notified, false => timeup
    auto wait_for(auto duration) -> bool {
        waked.access().second = false;
        auto lock             = std::unique_lock<std::mutex>(waked.get_raw_mutex());
        return condv.wait_for(lock, duration, [this]() { return waked.assume_locked(); });
    }

    auto wakeup() -> void {
        waked.access().second = true;
        condv.notify_all();
    }
};
} // namespace

#ifdef CUTIL_NS
}
#endif
