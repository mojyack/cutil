#pragma once
#include <print>
#include <span>

inline auto dump_hex(const std::span<const std::byte> data) -> void {
    for(auto i = 0uz; i < data.size();) {
        std::print("{:04x} ", i);
        auto ascii = std::array<char, 16 + 1>();
        for(auto l = 0; l < 2; l += 1) {
            for(auto c = l * 8; c < (l + 1) * 8; c += 1, i += 1) {
                if(i < data.size()) {
                    const auto bin = char(data[i]);
                    std::print("{:02x} ", bin);
                    ascii[c] = isprint(bin) != 0 ? bin : '.';
                } else {
                    std::print("   ");
                    ascii[c] = ' ';
                }
            }
            std::print(" ");
        }
        std::println("|{}|", ascii.data());
    }
}
