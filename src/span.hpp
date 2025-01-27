#pragma once
#include <span>
#include <string_view>

#define CUTIL_MODULE_NAME cutil_span_v2
#include "_prologue.hpp"

template <class T = std::byte>
    requires(sizeof(T) == sizeof(char))
auto to_span(const std::string_view str) -> std::span<const T> {
    return std::span(std::bit_cast<const T*>(str.data()), str.size());
}

template <class T>
auto from_span(const T& data) -> std::string_view {
    return std::string_view(std::bit_cast<const char*>(data.data()), data.size());
}

#include "_prologue.hpp"
