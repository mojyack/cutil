#include "assert.hpp"
#include "charconv.hpp"

auto main() -> int {
    dynamic_assert(from_chars<int>("0") == 0);
    dynamic_assert(from_chars<int>("-1") == -1);
    dynamic_assert(from_chars<int>("1") == 1);
    dynamic_assert(from_chars<int>("8086", 16) == 0x8086);
    // dynamic_assert(from_chars<double>("0.1") == 0.1); // not supported yet by clang-17
    std::println("pass");
    return 0;
}
