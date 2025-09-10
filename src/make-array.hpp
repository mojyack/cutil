#pragma once
#include <array>
#include <cstddef>

template <class T, size_t N>
constexpr auto make_array(T (&&values)[N]) -> std::array<T, N> {
    return std::to_array(values);
}
