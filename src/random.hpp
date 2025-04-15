#pragma once
#include <array>
#include <random>
#include <span>
#include <type_traits>
#include <vector>

struct RandomEngine {
    std::mt19937_64 engine;

    template <std::integral T>
    auto random_fill_bulk(T* const arr, const size_t count) -> void {
        for(auto i = 0uz; i < count; i += 1) {
            arr[i] = engine();
        }
    }

    template <size_t len>
    auto random_fill_fixed_len(std::byte* const ptr) -> void {
        using T8  = uint8_t;
        using T16 = std::conditional_t<(len >= sizeof(uint16_t)), uint16_t, T8>;
        using T32 = std::conditional_t<(len >= sizeof(uint32_t)), uint32_t, T16>;
        using T64 = std::conditional_t<(len >= sizeof(uint64_t)), uint64_t, T32>;
        using T   = T64;
        static_assert(len % sizeof(T) == 0);

        random_fill_bulk<T>(std::bit_cast<T*>(ptr), len / sizeof(T));
    }

    auto random_fill(std::span<std::byte> data) -> void {
        auto ptr = data.data();
        auto len = data.size();

#pragma push_macro("IF")
#define IF(T)                                               \
    if(len >= sizeof(T)) {                                  \
        const auto count = len / sizeof(T);                 \
        random_fill_bulk<T>(std::bit_cast<T*>(ptr), count); \
        ptr = ptr + count * sizeof(T);                      \
        len = len % sizeof(T);                              \
    }
        IF(uint64_t)
        IF(uint32_t)
        IF(uint16_t)
        IF(uint8_t)
#pragma pop_macro("IF")
    }

    template <size_t len>
    auto generate() -> std::array<std::byte, len> {
        auto ret = std::array<std::byte, len>();
        random_fill_fixed_len<len>(ret.data());
        return ret;
    }

    auto generate(const size_t len) -> std::vector<std::byte> {
        auto ret = std::vector<std::byte>(len);
        random_fill(ret);
        return ret;
    }

    RandomEngine()
        : engine((std::random_device())()) {
    }
};
