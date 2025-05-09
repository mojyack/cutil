#pragma once
#include <print>
#include <span>

template <class T>
inline auto dump_hex(const T& data) -> void {
    static_assert(sizeof(data[0]) == 1);
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

inline auto dump_hex(const void* const ptr, const size_t len) -> void {
    dump_hex(std::span{std::bit_cast<std::byte*>(ptr), len});
}
