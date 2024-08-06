#pragma once
#include <charconv>
#include <optional>
#include <string_view>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#else
namespace {
#endif

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
    auto r = T();
    if(auto [ptr, ec] = std::from_chars(str.begin(), str.end(), r); ec == std::errc{}) {
        return r;
    } else {
        return std::nullopt;
    }
}
}
