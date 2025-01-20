#pragma once
#include <print>
#include <string>

#define CUTIL_MODULE_NAME cutil_assert_v3
#include "_prologue.hpp"

template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    auto message = std::string();
    if constexpr(sizeof...(args) != 0) {
        message = std::format(std::forward<Args>(args)...);
    } else {
        message = "panic";
    }
#ifdef CUTIL_EXCEPTION
    throw std::runtime_error(message);
#else
    std::println(stderr, "{}", message);
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
