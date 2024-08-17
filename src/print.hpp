#pragma once
#include <iostream>
#include <sstream>

#define CUTIL_MODULE_NAME cutil_print_v1
#include "_prologue.hpp"

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

#include "_prologue.hpp"
