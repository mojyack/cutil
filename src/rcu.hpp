#pragma once
#include <atomic>
#include <thread>
#include <utility>

#define CUTIL_MODULE_NAME cutil_rcu_v1
#include "_prologue.hpp"

template <class T>
struct RCU {
  private:
    struct RCUData {
        T                    data     = {};
        std::atomic_uint32_t refcount = 0;

        // guard for worst case:
        // in emplace(), if refcount==0 check was done between reader's storage slot selection(auto& front = ...) and refcount increment in lock(),
        // that reader may see constructing(incomplete) data.
        // in such a rare case, that reader must wait for the complete flag.
        std::atomic_uint32_t complete = 1;

        auto unlock() -> void {
            refcount.fetch_sub(1);
        }
    };

    struct AutoLock {
        RCUData* data;

        ~AutoLock() {
            data->unlock();
        }
    };

    RCUData         storage[2];
    std::atomic_int flip = 0;

  public:
    auto copy_data() const -> T {
        return storage[flip].data;
    }

    template <class... Args>
    auto emplace(Args&&... args) -> size_t {
        auto& back = storage[!flip];

        auto spins = 0;
        while(back.refcount != 0) {
            std::this_thread::yield();
            spins += 1;
        }

        back.complete = 0;
        back.data     = T(std::forward<Args>(args)...);
        back.complete = 1;
        flip ^= 1;

        return spins;
    }

    auto lock() -> RCUData* {
        auto& front = storage[flip];
        front.refcount.fetch_add(1);

    loop:
        if(front.complete == 1) [[likely]] {
            return &front;
        }
        std::this_thread::yield();
        goto loop;
    }

    auto access() -> std::pair<AutoLock&&, T&> {
        auto data = lock();
        return {AutoLock{data}, data->data};
    }
};

#include "_prologue.hpp"
