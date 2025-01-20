#include <print>

#include "split.hpp"

auto main() -> int {
    {
        const auto elms = split("a b c", " ");
        const auto ok   = elms.size() == 3 && elms[0] == "a" && elms[1] == "b" && elms[2] == "c";
        std::println("{}", ok ? "ok" : "fail");
    }
    {
        const auto elms = split_like_shell(R"(a "b c" 'd e')");
        const auto ok   = elms.size() == 3 && elms[0] == "a" && elms[1] == "b c" && elms[2] == "d e";
        std::println("{}", ok ? "ok" : "fail");
    }
    return 0;
}
