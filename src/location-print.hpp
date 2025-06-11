#pragma once
#include <print>

#include "comptime-string.hpp"

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

constexpr auto is_clang =
#if defined(__clang__)
    true;
#else
    false;
#endif

// ugly compiler-dependent hack
template <comptime::String function_name>
constexpr auto format_function_name() -> auto {
    // macros to shadow constexpr variable
#pragma push_macro("tmp")
#pragma push_macro("end")
#undef tmp
#define tmp                    \
    constexpr auto str = str_; \
    {                          \
        constexpr auto str_
#undef end
#define end }

    constexpr auto str_ = function_name;

    // "static|virtual void Struct::func()"
    //  ^^^^^^^^^^^^^^^
    tmp = comptime::remove_prefix<str, "static ">;
    tmp = comptime::remove_prefix<str, "virtual ">;

    // "void Struct<int,char>::func(int) const [T = int, U = char]"
    //                                        ^^^^^^^^^^^^^^^^^^^^
    tmp = comptime::remove_region<str, '[', ']'>;
    tmp = comptime::remove_suffix<str, " ">;

    // "void Struct<int,char>::func(int) const""
    //                                  ^^^^^^
    tmp = comptime::remove_suffix<str, " const">;

    // "void Struct<int,char>::func(int)""
    //                             ^^^^^
    tmp = remove_suffix_pair<str, "(", ")">;

    // "int main()::(anonymous class)::operator()"
    //              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ -> <lambda>
    constexpr auto orig = str_;

    tmp = comptime::conditional<is_clang, comptime::remove_suffix<orig, "(anonymous class)::operator()">, remove_suffix_pair<orig, "<lambda", ">">>;
    tmp = comptime::conditional<(str.size() != orig.size()), comptime::concat<str, "{lambda}">, str>;

    // "void Struct<int,char>::func"
    //             ^^^^^^^^^^
    tmp = comptime::remove_region<str, '<', '>'>;

    // "void (anonymous namespace)::func"
    //       ^^^^^^^^^^^^^^^^^^^^^^^
    constexpr auto anon_label = std::string_view(is_clang ? "(anonymous namespace)::" : "{anonymous}::");

    tmp = comptime::replace<str, comptime::String<anon_label.size()>(anon_label), "">;

    // "Struct::func(std::optional<std::size_t>)::<lambda>"
    //              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    tmp = comptime::remove_region<str, '(', ')'>;

    // "const int *const *func"
    //  ^^^^^^^^^^^^^^^^^^
    // hack to compare npos-able values; since npos = (size_t)-1, so npos + 1 == 0 and 0 - 1 == npos
    constexpr auto pos = std::max(comptime::rfind<str_, " "> + 1, comptime::rfind<str_, "*"> + 1) - 1;

    tmp = comptime::conditional<(pos != std::string_view::npos), comptime::substr<str, pos + 1>, str>;
    tmp = comptime::remove_prefix<str, "*">;

    // "ns::ns2::ns3::func"
    //  ^^^^^^^^^
    tmp = remove_prefix_before_second_delim<str, "::">;
    return str_;

    end end end end end end end end end end end end end end;
#pragma pop_macro("tmp")
#pragma pop_macro("end")
}

template <comptime::String function_name>
constexpr auto format_file_name() -> auto {
    return remove_prefix_before_second_delim<function_name, comptime::String("/")>;
}
} // namespace cutil_impl

template <comptime::String filename, comptime::String function, size_t line, bool err, class... Args>
auto location_print(const std::format_string<Args...> format, Args&&... args) -> void {
    constexpr auto short_filename = cutil_impl::format_file_name<filename>();
    constexpr auto short_function = cutil_impl::format_function_name<function>();

    const auto out = err ? stderr : stdout;
    std::print(out, "{} @ {}:{} ", short_function.str(), short_filename.str(), line);
    std::println(out, format, std::forward<Args>(args)...);
}
