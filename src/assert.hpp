#pragma once
#include <print>
#include <string>

#define CUTIL_MODULE_NAME cutil_assert_v3
#include "_prologue.hpp"

template <class... Args>
[[noreturn]] auto panic(const std::format_string<Args...> format = "", Args... args) -> void {
#ifdef CUTIL_EXCEPTION
    throw std::runtime_error(std::format(format, std::forward<Args>(args)...));
#else
    std::println(stderr, format, std::forward<Args>(args)...);
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
