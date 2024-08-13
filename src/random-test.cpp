#include "print.hpp"
#include "random.hpp"

namespace {
auto print_bytes(const std::span<const std::byte> data) -> void {
    for(auto b : data) {
        printf("%02X", int(b));
    }
    printf("\n");
}
} // namespace

auto main(int argc, char** argv) -> int {
    if(argc < 2) {
        return 1;
    }

    auto engine = RandomEngine();
    if(const auto command = std::string_view(argv[1]); command == "1") {
        print("random test");
        auto v = std::vector<std::byte>();
        for(auto i = 0; i <= 64; i += 1) {
            printf("%02d: ", i);
            v.resize(i);
            engine.random_fill(v);
            print_bytes(v);
        }
        print_bytes(engine.generate(32));
        print_bytes(engine.generate<32>());
    } else if(command == "2") {
        print("benchmark");
        auto v = std::array<std::byte, 32>();
        for(auto i = 0; i <= 500000000; i += 1) {
            engine.random_fill_fixed_len<32>(v.data());
        }
    } else {
        return 1;
    }

    return 0;
}
