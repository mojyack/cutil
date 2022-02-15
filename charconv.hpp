#pragma once
#include <charconv>
#include <optional>
#include <string_view>

template <class T>
auto from_chars(const std::string_view& str) -> std::optional<T> {
    auto r = T();
    if(auto [ptr, ec] = std::from_chars(str.begin(), str.end(), r); ec == std::errc{}) {
        return r;
    } else {
        return std::nullopt;
    }
}
