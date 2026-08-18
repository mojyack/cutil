#pragma once
#include <array>
#include <span>
#include <vector>

using BytesVec     = std::vector<std::byte>;
using BytesSpan    = std::span<const std::byte>;
using BytesMutSpan = std::span<std::byte>;

template <size_t N>
using BytesArray = std::array<std::byte, N>;
template <size_t N>
using BytesRef = std::span<const std::byte, N>;
template <size_t N>
using BytesMutRef = std::span<std::byte, N>;
