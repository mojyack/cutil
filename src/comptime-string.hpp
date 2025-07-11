#pragma once
#include <algorithm>
#include <array>
#include <string_view>

namespace comptime {
template <size_t N>
struct String {
    std::array<char, N> data;

    constexpr auto operator[](const int i) const -> char {
        if(i >= 0) {
            return data[i];
        } else {
            return data[size() + i];
        }
    }

    constexpr auto size() const -> size_t {
        return data.size();
    }

    constexpr auto empty() const -> bool {
        return size() == 0;
    }

    constexpr auto str() const -> std::string_view {
        return {data.data(), data.size()};
    }

    constexpr String() {
    }

    constexpr String(const char c) {
        static_assert(N == 1);
        data[0] = c;
    }

    constexpr String(const char (&str)[N + 1]) {
        std::copy(std::begin(str), std::end(str) - 1, data.begin());
    }

    constexpr String(const std::string_view str) {
        // static_assert(N == str.size());
        std::copy(str.begin(), str.begin() + N, data.begin());
    }
};

template <size_t N>
String(const char (&)[N]) -> String<N - 1>;

// free utilities
template <String a, String b, String... rests>
constexpr auto concat_fn() -> auto {
    if constexpr(sizeof...(rests) == 0) {
        auto ret = String<a.size() + b.size()>();
        std::ranges::copy(a.data, ret.data.begin());
        std::ranges::copy(b.data, ret.data.begin() + a.size());
        return ret;
    } else {
        return concat_fn<concat_fn<a, b>(), rests...>();
    }
}
template <String a, String b, String... rests>
constexpr auto concat = concat_fn<a, b, rests...>();

template <bool cond, String a, String b>
constexpr auto conditional_fn() -> auto {
    if constexpr(cond) {
        return a;
    } else {
        return b;
    }
}
template <bool cond, String a, String b>
constexpr auto conditional = conditional_fn<cond, a, b>();

// methods
template <String str, String key>
constexpr auto starts_with_fn() -> bool {
    return str.str().starts_with(key.str());
}
template <String str, String key>
constexpr auto starts_with = starts_with_fn<str, key>();

template <String str, String key>
constexpr auto ends_with_fn() -> bool {
    return str.str().ends_with(key.str());
}
template <String str, String key>
constexpr auto ends_with = ends_with_fn<str, key>();

template <String str, size_t index, size_t len>
constexpr auto substr_fn() -> String<len> {
    auto       ret   = String<len>();
    const auto begin = str.data.begin() + index;
    std::ranges::copy(begin, begin + len, ret.data.begin());
    return ret;
}
template <String str, size_t index, size_t len = str.size() - index>
constexpr auto substr = substr_fn<str, index, len>();

template <String str, String key, size_t pos>
constexpr auto find_fn() -> std::size_t {
    return str.str().find(key.str(), pos);
}
template <String str, String key, size_t pos = 0>
constexpr auto find = find_fn<str, key, pos>();

template <String str, String key, size_t pos>
constexpr auto rfind_fn() -> std::size_t {
    return str.str().rfind(key.str(), pos);
}
template <String str, String key, size_t pos = std::string_view::npos>
constexpr auto rfind = rfind_fn<str, key, pos>();

template <String str, String prefix>
constexpr auto remove_prefix_fn() -> auto {
    if constexpr(starts_with<str, prefix>) {
        return substr<str, prefix.size()>;
    } else {
        return str;
    }
}
template <String str, String prefix>
constexpr auto remove_prefix = remove_prefix_fn<str, prefix>();

template <String str, String suffix>
constexpr auto remove_suffix_fn() -> auto {
    if constexpr(ends_with<str, suffix>) {
        return substr<str, 0, str.size() - suffix.size()>;
    } else {
        return str;
    }
}
template <String str, String suffix>
constexpr auto remove_suffix = remove_suffix_fn<str, suffix>();

template <String str, String from, String to, size_t index>
constexpr auto replace_fn() -> auto {
    constexpr auto found = find<str, from, index>;
    if constexpr(found != std::string_view::npos) {
        constexpr auto before   = substr<str, 0, found>;
        constexpr auto after    = substr<str, found + from.size()>;
        constexpr auto replaced = concat<before, to, after>;
        return replace_fn<replaced, from, to, found + to.size()>();
    } else {
        return str;
    }
}
template <String str, String from, String to, size_t index = 0>
constexpr auto replace = replace_fn<str, from, to, index>();

template <String str, char open, char close>
constexpr auto find_region_fn() -> std::pair<size_t, size_t> {
    auto cursor = 0uz;
    auto depth  = 0uz;
    auto anchor = std::string_view::npos;
    while(cursor < str.size()) {
        if(str[cursor] == open) {
            depth += 1;
            if(depth == 1) {
                anchor = cursor;
            }
        } else if(str[cursor] == close) {
            if(depth == 1) {
                return {anchor, cursor - anchor + 1};
            } else if(depth > 0) {
                depth -= 1;
            }
        }
        cursor += 1;
    }
    return {std::string_view::npos, 0};
}
template <String str, char open, char close>
constexpr auto find_region = find_region_fn<str, open, close>();

template <String str, char open, char close>
constexpr auto remove_region_fn() -> auto {
    constexpr auto region = find_region<str, open, close>;
    if constexpr(region.first == std::string_view::npos) {
        return str;
    } else {
        constexpr auto a = substr<str, 0, region.first>;
        // constexpr auto b  = substr<str, region.first, region.second>;
        constexpr auto c  = substr<str, region.first + region.second>;
        constexpr auto ac = concat<a, c>;
        return remove_region_fn<ac, open, close>();
    }
}
template <String str, char open, char close>
constexpr auto remove_region = remove_region_fn<str, open, close>();
} // namespace comptime
