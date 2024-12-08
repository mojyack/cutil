#pragma once
#include <cstdint>

#include "comptime-string.hpp"

#define CUTIL_MODULE_NAME cutil_comptime_string_v1
#include "_prologue.hpp"

namespace comptime {
template <uintmax_t num, String str = "">
constexpr auto to_string_unsinged_fn() -> auto {
    if constexpr(num == 0) {
        return conditional<!str.empty(), str, "0">;
    } else {
        constexpr auto c = '0' + num % 10;
        return to_string_unsinged_fn<num / 10, concat<String<1>(c), str>>();
    }
}
template <auto num>
constexpr auto to_string_unsinged = to_string_unsinged_fn<num>();

template <auto num>
constexpr auto to_string_fn() -> auto {
    if constexpr(num >= 0) {
        return to_string_unsinged<num>;
    } else {
        return concat<String("-"), to_string_unsinged<-num>>;
    }
}
template <auto num>
constexpr auto to_string = to_string_fn<num>();
} // namespace comptime

#include "_prologue.hpp"
