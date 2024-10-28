#pragma once
#include <source_location>
#include <string>

#include "print.hpp"

#define CUTIL_MODULE_NAME cutil_assert_v3
#include "_prologue.hpp"

namespace impl {
inline auto remove_prefix(std::string& str, const std::string_view prefix) -> void {
    if(str.starts_with(prefix)) {
        str = str.substr(prefix.size());
    }
}

// a::b::c::d -> c::d
inline auto remove_prefix_before_second_delim(std::string& str, const std::string_view dlm) -> void {
    const auto p1 = str.rfind(dlm);
    if(p1 == str.npos) return;
    const auto p2 = str.rfind(dlm, p1 - 1);
    if(p2 == str.npos) return;
    str = str.substr(p2 + dlm.size());
}

inline auto remove_suffix(std::string& str, const std::string_view suffix) -> void {
    if(str.ends_with(suffix)) {
        str.resize(str.size() - suffix.size());
    }
}

inline auto remove_suffix_pair(std::string& str, const char open, const char close) -> void {
    if(str.back() != close) return;
    const auto p = str.rfind(open);
    if(p == str.npos) return;
    str.resize(p);
}

// ugly compiler-dependent hack
inline auto format_function_name(std::string name) -> std::string {
    remove_prefix(name, "static ");
    remove_prefix(name, "virtual ");

    remove_suffix_pair(name, '[', ']'); // template parameters
    remove_suffix(name, " ");
    remove_suffix(name, " const");
    remove_suffix_pair(name, '(', ')'); // argument list

    // remove return type
    for(auto i = 0, depth = 0; i < int(name.size()); i += 1) {
        if(name[i] == '<') {
            depth += 1;
        } else if(name[i] == '>') {
            depth -= 1;
        } else if(name[i] == ' ' && depth == 0) {
            name = name.substr(i + 1);
            break;
        }
    }

    remove_prefix_before_second_delim(name, "::"); // remove deep namespace
    return name;
}

inline auto format_file_name(std::string name) -> std::string {
    remove_prefix_before_second_delim(name, "/");
    return name;
}
} // namespace impl

template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    auto message = std::string("panic");
    if constexpr(sizeof...(args) != 0) {
        message = build_string(std::forward<Args>(args)...);
    }
#ifdef CUTIL_EXCEPTION
    throw std::runtime_error(message);
#else
    warn(message);
    exit(1);
#endif
}

template <class... Args>
auto dynamic_assert(const bool cond, Args... args) -> void {
    if(!cond) {
        panic(args...);
    }
}

using Location = std::source_location;

template <class... Args>
struct line_panic {
    line_panic(Args&&... args, const Location location = Location::current()) {
        const auto message = build_string("fatal error at \"", location.function_name(), "\" at ", location.file_name(), ":", location.line(), " ", std::forward<Args>(args)...);
#ifdef CUTIL_EXCEPTION
        throw std::runtime_error(message);
#else
        warn(message);
        exit(1);
#endif
    }
};

template <class... Args>
line_panic(Args&&... args) -> line_panic<Args...>;

template <class... Args>
struct line_print {
    line_print(Args&&... args, const Location location = Location::current()) {
        print(impl::format_function_name(location.function_name()), " @ ",
              impl::format_file_name(location.file_name()), ":",
              location.line(), " ",
              std::forward<Args>(args)...);
    }
};

template <class... Args>
line_print(Args&&... args) -> line_print<Args...>;

template <class... Args>
struct line_warn {
    line_warn(Args&&... args, const Location location = Location::current()) {
        warn(impl::format_function_name(location.function_name()), " @ ",
             impl::format_file_name(location.file_name()), ":",
             location.line(), " ",
             std::forward<Args>(args)...);
    }
};

template <class... Args>
line_warn(Args&&... args) -> line_warn<Args...>;

template <class... Args>
struct line_assert {
    line_assert(const bool cond, Args&&... args, const Location location = Location::current()) {
        if(!cond) {
            line_panic<Args...>(std::forward<Args>(args)..., location);
        }
    }
};

template <class... Args>
line_assert(const bool cond, Args&&... args) -> line_assert<Args...>;

#include "_prologue.hpp"
