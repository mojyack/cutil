#pragma once
#include <print>
#include <string>

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
auto dynamic_assert(const bool cond, const std::format_string<Args...> format = "", Args... args) -> void {
    if(!cond) {
        panic(format, args...);
    }
}
