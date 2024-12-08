#pragma once
#include <string>

#include "print.hpp"

#define CUTIL_MODULE_NAME cutil_assert_v3
#include "_prologue.hpp"

template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    auto message = std::string("panic");
    if constexpr(sizeof...(args) != 0) {
        message = build_string(std::forward<Args>(args)...);
    }
#ifdef CUTIL_EXCEPTION
    throw std::runtime_error(message);
#else
    warn(message);
    exit(1);
#endif
}

template <class... Args>
auto dynamic_assert(const bool cond, Args... args) -> void {
    if(!cond) {
        panic(args...);
    }
}

#include "_prologue.hpp"
