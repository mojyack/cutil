#pragma once
#include <iostream>
#include <sstream>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

namespace {
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
} // namespace

#ifdef CUTIL_NS
}
#endif
