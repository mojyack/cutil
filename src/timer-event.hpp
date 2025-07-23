#pragma once
#include <condition_variable>

#include "critical.hpp"

struct TimerEvent {
    std::condition_variable cv;
    Critical<bool>          flag;

    auto clear() -> void {
        flag.access().second = false;
    }

    auto wait() -> void {
        auto lock = std::unique_lock<std::mutex>(flag.get_raw_mutex());
        cv.wait(lock, [this]() { return flag.assume_locked(); });
    }

    // returns: true => notified, false => timeup
    auto wait_for(auto duration) -> bool {
        auto lock = std::unique_lock<std::mutex>(flag.get_raw_mutex());
        return cv.wait_for(lock, duration, [this]() { return flag.assume_locked(); });
    }

    auto notify() -> void {
        flag.access().second = true;
        cv.notify_all();
    }
};
