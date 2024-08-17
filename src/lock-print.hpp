#pragma once
#include <iostream>

#define CUTIL_MODULE_NAME cutil_lock_print_v1
#include "_prologue.hpp"

inline auto cout_lock = std::mutex();

template <class... Args>
auto lock_print(Args... args) -> void {
    cout_lock.lock();
    (std::cout << ... << args) << std::endl;
    cout_lock.unlock();
}

#include "_prologue.hpp"
