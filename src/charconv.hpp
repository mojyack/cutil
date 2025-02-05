#pragma once
#include <charconv>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

#define CUTIL_MODULE_NAME cutil_charconv_v1
#include "_prologue.hpp"

template <std::integral T>
auto from_chars(const std::string_view str, const int base = 10) -> std::optional<T> {
    auto r = T();
    if(auto [ptr, ec] = std::from_chars(str.begin(), str.end(), r, base); ec == std::errc{}) {
        return r;
    } else {
        return std::nullopt;
    }
}

template <std::floating_point T>
auto from_chars(const std::string_view str) -> std::optional<T> {
    errno = 0;
    if(const auto r = std::strtod(std::string(str).data(), NULL); errno == 0) {
        return r;
    } else {
        return std::nullopt;
    }

    /* clang-19 does not suport from_chars<double> yet
    auto r = T();
    if(auto [ptr, ec] = std::from_chars(str.begin(), str.end(), r); ec == std::errc{}) {
        return r;
    } else {
        return std::nullopt;
    }
    */
}

#include "_prologue.hpp"
