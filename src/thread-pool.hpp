#pragma once
#include <array>
#include <thread>

#include "multi-event.hpp"

#define CUTIL_MODULE_NAME cutil_string_map_v1
#include "_prologue.hpp"

template <int N>
struct ThreadPool {
    std::atomic_int            finished;
    std::array<std::thread, N> threads;
    MultiEvent                 event;

    auto run(const auto target) -> void {
        for(auto& thread : threads) {
            thread = std::thread([this, target]() {
                target();
                finished.fetch_add(1);
            });
        }
    }

    auto stop() -> void {
        while(finished != N) {
            event.notify_unblock();
            std::this_thread::yield();
        }
        for(auto& thread : threads) {
            thread.join();
        }
    }
};

template <class T, int N>
struct CustomDataThreadPool : ThreadPool<N> {
    std::array<T, N> data;

    auto run(const auto target) -> void {
        for(auto i = 0; i < N; i += 1) {
            this->threads[i] = std::thread([this, target, i]() -> void {
                target(data[i]);
                this->finished.fetch_add(1);
            });
        }
    }
};

#include "_prologue.hpp"
