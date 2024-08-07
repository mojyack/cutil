#pragma once
#include <iostream>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#else
namespace {
#endif

inline auto cout_lock = std::mutex();

template <class... Args>
auto lock_print(Args... args) -> void {
    cout_lock.lock();
    (std::cout << ... << args) << std::endl;
    cout_lock.unlock();
}
}
