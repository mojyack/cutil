#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class T>
struct Critical {
    mutable std::mutex mutex;
    T                  data;

    auto get_lock() const -> std::lock_guard<std::mutex> {
        return std::lock_guard<std::mutex>(mutex);
    }
    auto store(T src) -> void {
        const auto lock = get_lock();
        data            = src;
    }
    auto load() const -> T {
        const auto lock = get_lock();
        return data;
    }
    auto replace(T src = T()) -> T {
        const auto lock = get_lock();
        std::swap(data, src);
        return src;
    }
    auto operator->() -> T* {
        return &data;
    }
    auto operator->() const -> const T* {
        return &data;
    }
    auto operator*() -> T& {
        return data;
    }
    auto operator*() const -> const T& {
        return data;
    }
    Critical(T src) : data(src) {}
    Critical() {}
};

class Event {
  private:
    std::atomic_flag flag;

  public:
    auto wait() -> void {
        flag.clear();
        flag.wait(false);
    }
    auto wakeup() -> void {
        flag.test_and_set();
        flag.notify_all();
    }
    Event() {}
    Event(const Event&) {}
};

class TimerEvent {
  private:
    std::condition_variable condv;
    Critical<bool>          waked;

  public:
    auto wait() -> void {
        waked.store(false);
        auto lock = std::unique_lock<std::mutex>(waked.mutex);
        condv.wait(lock, [this]() { return waked.data; });
    }
    auto wait_for(auto duration) -> bool {
        waked.store(false);
        auto lock = std::unique_lock<std::mutex>(waked.mutex);
        return condv.wait_for(lock, duration, [this]() { return waked.data; });
    }
    auto wakeup() -> void {
        waked.store(true);
        condv.notify_all();
    }
};

#ifdef CUTIL_NS
}
#endif
