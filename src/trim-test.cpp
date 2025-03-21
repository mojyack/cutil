#include <print>

#include "trim.hpp"

namespace {
auto errors = 0uz;

auto test(std::string_view a, std::string_view b) -> void {
    if(a != b) {
        std::println(stderr, "result did not match '{}' != '{}'", a, b);
        errors += 1;
    }
}
} // namespace

auto main() -> int {
    test(ltrim(" \n\r\t\v hello"), "hello");
    test(rtrim("hello \n\r\t\v "), "hello");
    test(trim(" \n\r\t\v hello world \n\r\t\v "), "hello world");
    if(errors == 0) std::println("pass");
    return 0;
}
