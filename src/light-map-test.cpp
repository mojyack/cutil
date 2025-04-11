#include <print>

#include "light-map.hpp"

#define ensure(cond)                  \
    if(!(cond)) {                     \
        std::println("{}", __LINE__); \
        return 1;                     \
    }

auto main() -> int {
    auto map = LightMap<int, int>();
    map[0]   = 0;
    map[1]   = 1;
    map[2]   = 2;
    ensure(map[0] == 0);
    ensure(map.contains(1));
    ensure(map.find(2)->second == 2);
    ensure(map.find(3) == map.end());
    map.erase(map.find(1));
    ensure(map.find(1) == map.end());
    map.erase(2);
    ensure(map.find(2) == map.end());
    std::println("pass");
    return 0;
}
