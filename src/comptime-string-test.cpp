#include "comptime-charconv.hpp"

auto main() -> int {
    constexpr auto str1 = comptime::String("hello");
    static_assert(str1.size() == 5);
    static_assert(str1.str() == "hello");

    constexpr auto str2 = comptime::concat<str1, comptime::String(" "), comptime::String("world")>;
    static_assert(str2.str() == "hello world");

    static_assert(comptime::starts_with<str2, comptime::String("hello")>);
    static_assert(!comptime::starts_with<str2, comptime::String("hello!")>);
    static_assert(comptime::ends_with<str2, comptime::String("world")>);
    static_assert(!comptime::ends_with<str2, comptime::String("hello!")>);

    static_assert(comptime::substr<str2, 1, 9>.str() == "ello worl");
    static_assert(comptime::substr<str2, 6>.str() == "world");

    static_assert(comptime::find<str2, comptime::String("o")> == 4);
    static_assert(comptime::find<str2, comptime::String("o"), 5> == 7);
    static_assert(comptime::rfind<str2, comptime::String("o")> == 7);
    static_assert(comptime::rfind<str2, comptime::String("o"), 5> == 4);
    static_assert(comptime::find<str2, comptime::String("!")> == std::string_view::npos);

    static_assert(comptime::remove_prefix<str2, comptime::String("hello ")>.str() == "world");
    static_assert(comptime::remove_suffix<str2, comptime::String(" world")>.str() == "hello");

    static_assert(comptime::replace<str2, comptime::String(" "), comptime::String("___")>.str() == "hello___world");

    static_assert(comptime::to_string<65536>.str() == "65536");
    static_assert(comptime::to_string<0>.str() == "0");
    static_assert(comptime::to_string<-65536>.str() == "-65536");

    // there is no things to test at runtime, great!

    return 0;
}
