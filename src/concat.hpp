#pragma once
#include <cstring>
#include <vector>

inline auto copy(const auto a) -> std::vector<std::byte> {
    auto ret = std::vector<std::byte>(a.size());
    std::memcpy(ret.data(), a.data(), a.size());
    return ret;
}

inline auto concat(const auto& a, const auto& b) -> std::vector<std::byte> {
    auto ret = std::vector<std::byte>(a.size() + b.size());
    std::memcpy(ret.data(), a.data(), a.size());
    std::memcpy(ret.data() + a.size(), b.data(), b.size());
    return ret;
}
