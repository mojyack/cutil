#pragma once
#include <atomic>

#define CUTIL_MODULE_NAME cutil_event_v1
#include "_prologue.hpp"

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

#include "_prologue.hpp"
