#pragma once
#include <iostream>

#include "comptime-string.hpp"

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

template <comptime::String str, comptime::String open, comptime::String close>
constexpr auto remove_region_recursive_fn() -> auto {
    constexpr auto opos = comptime::find<str, open>;
    constexpr auto epos = comptime::find<str, close, opos + 1>;
    if constexpr(opos == std::string_view::npos || epos == std::string_view::npos) {
        return str;
    } else {
        constexpr auto before = comptime::substr<str, 0, opos>;
        constexpr auto after  = comptime::substr<str, epos + 1>;
        constexpr auto substr = comptime::concat<before, after>;
        return remove_region_recursive_fn<substr, open, close>();
    }
}
template <comptime::String str, comptime::String open, comptime::String close>
constexpr auto remove_region_recursive = remove_region_recursive_fn<str, open, close>();

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
    constexpr auto clang = is_clang();

    // "static|virtual void Struct::func()"
    //  ^^^^^^^^^^^^^^^
    constexpr auto str000 = function_name;
    constexpr auto str010 = comptime::remove_prefix<str000, "static ">;
    constexpr auto str020 = comptime::remove_prefix<str010, "virtual ">;

    // "void Struct::func(int) const [T = int, U = char]"
    //                              ^^^^^^^^^^^^^^^^^^^^
    constexpr auto str030 = remove_suffix_pair<str020, "[", "]">;
    constexpr auto str040 = comptime::remove_suffix<str030, " ">;

    // "void Struct::func(int) const""
    //                        ^^^^^^
    constexpr auto str050 = comptime::remove_suffix<str040, " const">;

    // "void Struct::func(int)""
    //                   ^^^^^
    constexpr auto str060 = remove_suffix_pair<str050, "(", ")">;

    // "int main()::(anonymous class)::operator()"
    //              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ -> <lambda>
    constexpr auto str070 = comptime::conditional<clang, comptime::remove_suffix<str060, "(anonymous class)::operator()">, remove_suffix_pair<str060, "<lambda", ">">>;
    constexpr auto str080 = comptime::conditional<str060.size() != str070.size(), comptime::concat<str070, "<lambda>">, str070>;

    // "void (anonymous namespace)::func"
    //       ^^^^^^^^^^^^^^^^^^^^^^^
    constexpr auto anon_label = std::string_view(clang ? "(anonymous namespace)::" : "{anonymous}::");
    constexpr auto str090     = comptime::replace<str080, comptime::String<anon_label.size()>(anon_label), "">;

    // "const int *const *func"
    //  ^^^^^^^^^^^^^^^^^^
    // hack to compare npos-able values; since npos = (size_t)-1, so npos + 1 == 0 and 0 - 1 == npos
    constexpr auto pos    = std::max(comptime::rfind<str090, " "> + 1, comptime::rfind<str090, "*"> + 1) - 1;
    constexpr auto str100 = comptime::conditional<pos != std::string_view::npos, comptime::substr<str090, pos + 1>, str090>;
    constexpr auto str110 = comptime::remove_prefix<str100, "*">;

    // "Struct::func(std::optional<std::size_t>)::<lambda>"
    //              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    constexpr auto str120 = remove_region_recursive<str110, "(", ")">;

    // "ns::ns2::ns3::func"
    //  ^^^^^^^^^
    constexpr auto str130 = remove_prefix_before_second_delim<str120, "::">;

    return str130;
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
