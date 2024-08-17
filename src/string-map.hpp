#pragma once
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#define CUTIL_MODULE_NAME cutil_string_map_v1
#include "_prologue.hpp"

namespace internal {
struct StringHash {
    using is_transparent = void;
    using hash_type      = std::hash<std::string_view>;

    auto operator()(const std::string_view str) const -> size_t {
        return hash_type{}(str);
    }

    auto operator()(const std::string& str) const -> size_t {
        return hash_type{}(str);
    }

    auto operator()(const char* const str) const -> size_t {
        return hash_type{}(str);
    }
};
} // namespace internal

// .insert() not supported yet(P2363)
template <class T>
using StringMap = std::unordered_map<std::string, T, internal::StringHash, std::ranges::equal_to>;

#include "_prologue.hpp"
