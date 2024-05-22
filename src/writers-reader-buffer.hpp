#pragma once
#include <vector>

#include "critical.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class T>
class WritersReaderBuffer {
  private:
    Critical<std::vector<T>> buffer[2];
    std::atomic_int          flip = 0;

  public:
    auto push(T item) -> void {
        auto [lock, data] = buffer[flip].access();
        data.push_back(std::move(item));
    }

    auto swap() -> std::vector<T>& {
        buffer[!flip].unsafe_access().clear();
        flip ^= 1;
        auto [lock, data] = buffer[!flip].access();
        return data;
    }
};
#ifdef CUTIL_NS
}
#endif
