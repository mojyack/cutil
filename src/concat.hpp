#pragma once
#include <cstring>
#include <vector>

#define CUTIL_MODULE_NAME cutil_assert_v1
#include "_prologue.hpp"

inline auto concat(const auto& a, const auto& b) -> std::vector<std::byte> {
    auto ret = std::vector<std::byte>(a.size() + b.size());
    std::memcpy(ret.data(), a.data(), a.size());
    std::memcpy(ret.data() + a.size(), b.data(), b.size());
    return ret;
}

#include "_prologue.hpp"
