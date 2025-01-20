#include <print>

#include "pair-table.hpp"

namespace {
enum Enum {
    A,
    B,
    C,
};

const auto table = make_pair_table<Enum, std::string_view>({
    {Enum::A, "A"},
    {Enum::C, "C"},
});

auto run() -> bool {
    if(const auto p = table.find("A"); !p || *p != Enum::A) {
        return false;
    }
    if(const auto p = table.find("B"); p) {
        return false;
    }
    if(const auto p = table.find("C"); !p || *p != Enum::C) {
        return false;
    }
    if(const auto p = table.find(Enum::A); !p || *p != "A") {
        return false;
    }
    if(const auto p = table.find(Enum::B); p) {
        return false;
    }
    if(const auto p = table.find(Enum::C); !p || *p != "C") {
        return false;
    }
    return true;
}
} // namespace

auto main() -> int {
    if(!run()) {
        std::println("fail");
        return 1;
    } else {
        std::println("pass");
        return 0;
    }
}
