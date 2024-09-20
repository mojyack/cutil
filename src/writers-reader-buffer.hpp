#pragma once
#include <atomic>
#include <vector>

#include "critical.hpp"

#define CUTIL_MODULE_NAME cutil_writers_reader_buffer_v2
#include "_prologue.hpp"

template <class T>
class WritersReaderBuffer {
  private:
    Critical<std::vector<T>> buffer[2];
    std::atomic_int          flip = 0;

  public:
    auto push(T item) -> void {
    retry:
        const auto front  = flip.load();
        auto [lock, data] = buffer[front].access();
        // reader may swapped buffer while aquireing the buffer lock
        if(front != flip) {
            goto retry;
        }

        data.push_back(std::move(item));
    }

    auto swap() -> std::vector<T>& {
        buffer[!flip].unsafe_access().clear();
        flip ^= 1;
        auto [lock, data] = buffer[!flip].access();
        return data;
    }
};

#include "_prologue.hpp"
