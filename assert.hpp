#pragma once
#include "print.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    const auto message = build_string(std::forward<Args...>(args...));
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

struct Location {
    const char* file;
    const int   line;
};

template <class... Args>
[[noreturn]] auto line_panic(const Location location, Args&&... args) -> void {
    const auto message = build_string("error at ", location.file, ":", location.line, " ", std::forward<Args>(args)...);
#ifdef CUTIL_EXCEPTION
    throw std::runtime_error(message);
#else
    warn(message);
    exit(1);
#endif
}

template <class... Args>
auto line_print(const Location location, Args&&... args) -> void {
    print(location.file, ":", location.line, " ", std::forward<Args>(args)...);
}

template <class... Args>
auto line_assert(const bool cond, const Location location, Args&&... args) -> void {
    if(!cond) {
        line_panic(location, std::forward<Args>(args)...);
    }
}

#ifdef CUTIL_NS
}
#endif
