#include "assert.hpp"

auto f() -> void {
    line_print("func");
}

auto a(int) -> void {
    line_print("func with args");
}

template <class T, class U>
auto t() -> void {
    line_print("template func");
}

auto s() -> std::optional<std::optional<const int>> {
    line_print("space in return type");
    return {};
}

auto p() -> int** {
    line_print("pointer return type");
    return nullptr;
}

struct S {
    static auto s() -> void {
        line_print("struct static");
    }

    auto f() -> void {
        line_print("struct method");
    }

    auto c() const -> void {
        line_print("struct const");
    }

    virtual auto v() -> void {
        line_print("struct virtual");
    }

    template <class T, class U>
    auto t(int) const -> void {
        line_print("struct template method");
    }

    S() {
        line_print("struct constructor");
    }

    ~S() {
        line_print("struct destructor");
    }
};

namespace ns {
auto f() -> void {
    line_print("namespace");
}

namespace ns2 {
auto f() -> void {
    line_print("nested namespace");
}
} // namespace ns2
} // namespace ns

auto main() -> int {
    f();
    a(0);
    t<int, std::basic_string<char>>();
    s();
    p();
    {
        auto s = S();
        s.f();
        s.s();
        s.c();
        s.v();
        s.t<int, char>(0);
    }
    ns::f();
    ns::ns2::f();
    auto l = []() {
        line_print("lambda");
    };
    l();
    return 0;
}
