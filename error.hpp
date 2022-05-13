#pragma once
#include <exception>
#include <iostream>
#include <sstream>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    auto ss = std::stringstream();
    (ss << ... << args) << std::endl;
    throw std::runtime_error(ss.str());
}

template <class... Args>
auto warn(Args... args) -> void {
    (std::cerr << ... << args) << std::endl;
}

template <class... Args>
auto print(Args... args) -> void {
    (std::cout << ... << args) << std::endl;
}

template <class... Args>
auto build_string(Args... args) -> std::string {
    auto ss = std::stringstream();
    (ss << ... << args);
    return ss.str();
}

template <class... Args>
auto dynamic_assert(const bool cond, Args... args) -> void {
    if(!cond) {
        panic(args...);
    }
}

#ifdef CUTIL_NS
}
#endif
