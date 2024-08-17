#pragma once
#include "event.hpp"

#define CUTIL_MODULE_NAME cutil_multi_event_v1
#include "_prologue.hpp"

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

    // blocks until at least waked up one thread
    auto notify() -> void {
        notified.test_and_set();
        notified.notify_all();

        all_waiters_synced.wait();

        notified.clear();
        notified.notify_all();
    }

    // if there are no waiters, returns immediately
    auto notify_unblock() -> void {
        if(total_waiters <= 0) {
            return;
        }
        notify();
    }

    MultiEvent() = default;

    MultiEvent(const MultiEvent&) {}
};

#include "_prologue.hpp"
