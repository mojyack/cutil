#include "make-array.hpp"

constexpr auto a = make_array<int>({1, 2, 3});
static_assert(a.size() == 3 && a[0] == 1 && a[1] == 2 && a[2] == 3);

struct S {
    int                a;
    double             b;
    std::array<int, 2> c;
};

constexpr auto b = make_array<S>({
    {.a = 1, .b = 1.0, .c = {1, 2}},
    {.a = 2, .b = 2.0, .c = {3, 4}},
});
static_assert(b.size() == 2);

auto main() -> int {
    return 0;
}
