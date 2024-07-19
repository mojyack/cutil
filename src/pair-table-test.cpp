#include "pair-table.hpp"
#include "print.hpp"

namespace {
enum Enum {
    A,
    B,
    C,
};

const auto table = make_pair_table<Enum, const char*>({
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
    if(const auto p = table.find(Enum::A); !p || std::string_view(*p) != "A") {
        return false;
    }
    if(const auto p = table.find(Enum::B); p) {
        return false;
    }
    if(const auto p = table.find(Enum::C); !p || std::string_view(*p) != "C") {
        return false;
    }
    return true;
}
} // namespace

auto main() -> int {
    if(!run()) {
        print("fail");
        return 1;
    } else {
        print("pass");
        return 0;
    }
}
