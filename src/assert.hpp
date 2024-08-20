#pragma once
#include <source_location>

#include "print.hpp"

#define CUTIL_MODULE_NAME cutil_assert_v2
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

using Location = std::source_location;

template <class... Args>
struct line_panic {
    line_panic(Args&&... args, const Location location = Location::current()) {
        const auto message = build_string("fatal error at \"", location.function_name(), "\" at ", location.file_name(), ":", location.line(), " ", std::forward<Args>(args)...);
#ifdef CUTIL_EXCEPTION
        throw std::runtime_error(message);
#else
        warn(message);
        exit(1);
#endif
    }
};

template <class... Args>
line_panic(Args&&... args) -> line_panic<Args...>;

template <class... Args>
struct line_print {
    line_print(Args&&... args, const Location location = Location::current()) {
        print("\"", location.function_name(), "\" at ", location.file_name(), ":", location.line(), " ", std::forward<Args>(args)...);
    }
};

template <class... Args>
line_print(Args&&... args) -> line_print<Args...>;

template <class... Args>
struct line_warn {
    line_warn(Args&&... args, const Location location = Location::current()) {
        warn("\"", location.function_name(), "\" at ", location.file_name(), ":", location.line(), " ", std::forward<Args>(args)...);
    }
};

template <class... Args>
line_warn(Args&&... args) -> line_warn<Args...>;

template <class... Args>
struct line_assert {
    line_assert(const bool cond, Args&&... args, const Location location = Location::current()) {
        if(!cond) {
            line_panic<Args...>(std::forward<Args>(args)..., location);
        }
    }
};

template <class... Args>
line_assert(const bool cond, Args&&... args) -> line_assert<Args...>;

#include "_prologue.hpp"
