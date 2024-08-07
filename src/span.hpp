#pragma once
#include <span>
#include <string_view>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#else
namespace {
#endif

inline auto to_span(const std::string_view str) -> std::span<const std::byte> {
    return std::span(std::bit_cast<const std::byte*>(str.data()), str.size());
}

inline auto from_span(std::span<const std::byte> data) -> std::string_view {
    return std::string_view(std::bit_cast<const char*>(data.data()), data.size());
}
}
