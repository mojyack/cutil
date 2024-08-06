#pragma once
#include <iostream>

namespace CUTIL_NS {
inline auto cout_lock = std::mutex();

template <class... Args>
auto lock_print(Args... args) -> void {
    cout_lock.lock();
    (std::cout << ... << args) << std::endl;
    cout_lock.unlock();
}
} // namespace CUTIL_NS
