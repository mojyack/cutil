#pragma once
#include <iostream>

#include "comptime-charconv.hpp"

#define CUTIL_MODULE_NAME cutil_location_print_v1
#include "_prologue.hpp"

namespace cutil_impl {
template <comptime::String str, comptime::String dlm>
constexpr auto remove_prefix_before_second_delim_fn() -> auto {
    const auto p1 = comptime::rfind<str, dlm>;
    const auto p2 = comptime::rfind<str, dlm, p1 - 1>;
    if constexpr(p1 == std::string_view::npos || p2 == std::string_view::npos) {
        return str;
    } else {
        return comptime::substr<str, p2 + dlm.size()>;
    }
}
template <comptime::String str, comptime::String dlm>
constexpr auto remove_prefix_before_second_delim = remove_prefix_before_second_delim_fn<str, dlm>();

template <comptime::String str, comptime::String open, comptime::String close>
constexpr auto remove_suffix_pair_fn() -> auto {
    constexpr auto p = comptime::rfind<str, open>;
    if constexpr(!comptime::ends_with<str, close> || p == std::string_view::npos) {
        return str;
    } else {
        return comptime::substr<str, 0, p>;
    }
}
template <comptime::String str, comptime::String open, comptime::String close>
constexpr auto remove_suffix_pair = remove_suffix_pair_fn<str, open, close>();

template <comptime::String str, int index = 0, int depth = 0>
constexpr auto remove_return_type_fn() -> auto {
    if constexpr(index < str.size()) {
        if constexpr(str[index] == ' ' && depth == 0) {
            return comptime::substr<str, index + 1>;
        } else if constexpr(str[index] == '<') {
            return remove_return_type_fn<str, index + 1, depth + 1>();
        } else if constexpr(str[index] == '>') {
            return remove_return_type_fn<str, index + 1, depth - 1>();
        } else {
            return remove_return_type_fn<str, index + 1, depth>();
        }
    } else {
        return str;
    }
}
template <comptime::String str, int index = 0, int depth = 0>
constexpr auto remove_return_type = remove_return_type_fn<str, index, depth>();

template <comptime::String str, comptime::String prefix>
constexpr auto remove_prefix_recursive_fn() -> auto {
    if constexpr(comptime::starts_with<str, prefix>) {
        return remove_prefix_recursive_fn<comptime::remove_prefix<str, prefix>, prefix>();
    } else {
        return str;
    }
}
template <comptime::String str, comptime::String prefix>
constexpr auto remove_prefix_recursive = remove_prefix_recursive_fn<str, prefix>();

constexpr auto is_clang() -> bool {
#if defined(__clang__)
    return true;
#else
    return false;
#endif
}

// ugly compiler-dependent hack
template <comptime::String function_name>
constexpr auto format_function_name() -> auto {
    using namespace comptime;

    constexpr auto clang = is_clang();

    constexpr auto str0 = function_name;
    constexpr auto str1 = remove_prefix<str0, String("static ")>;
    constexpr auto str2 = remove_prefix<str1, String("virtual ")>;

    constexpr auto str3 = remove_suffix_pair<str2, String("["), String("]")>; // template parameters
    constexpr auto str4 = remove_suffix<str3, String(" ")>;
    constexpr auto str5 = remove_suffix<str4, String(" const")>;
    constexpr auto str6 = remove_suffix_pair<str5, String("("), String(")")>; // argument list

    constexpr auto str7 = conditional<clang, remove_suffix<str6, String("(anonymous class)::operator()")>, remove_suffix_pair<str6, String("<lambda"), String(">")>>;
    constexpr auto str8 = conditional<str6.size() != str7.size(), concat<str7, String("<lambda>")>, str7>;

    constexpr auto str9  = remove_return_type<str8>;
    constexpr auto str10 = remove_prefix_recursive<str9, String("*")>; // remove pointer asterisk

    // now name should contains only namespace and function name
    constexpr auto anon_label = std::string_view(clang ? "(anonymous namespace)::" : "{anonymous}::");
    constexpr auto str11      = replace<str10, String<anon_label.size()>(anon_label), String("")>;
    constexpr auto str12      = remove_prefix_before_second_delim<str11, String("::")>;

    return str12;
}

template <comptime::String function_name>
constexpr auto format_file_name() -> auto {
    return remove_prefix_before_second_delim<function_name, comptime::String("/")>;
}
} // namespace cutil_impl

template <comptime::String filename, comptime::String function, size_t line, bool err, class... Args>
auto location_print(Args&&... args) -> void {
    constexpr auto short_filename = cutil_impl::format_file_name<filename>();
    constexpr auto short_function = cutil_impl::format_function_name<function>();

    auto& out = err ? std::cerr : std::cout;
    out << short_function.str() << " @ " << short_filename.str() << ":" << line << " ";
    (out << ... << args) << std::endl;
}

#include "_prologue.hpp"
