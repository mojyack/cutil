#pragma once
#include <array>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

namespace {
template <class T, class U, size_t N>
struct PairTable {
    using Pair = std::pair<T, U>;

    std::array<Pair, N> array;

    auto find(const T value) const -> const U* {
        for(const auto& e : array) {
            if(e.first == value) {
                return &e.second;
            }
        }
        return nullptr;
    }

    auto find(const U value) const -> const T* {
        for(const auto& e : array) {
            if(e.second == value) {
                return &e.first;
            }
        }
        return nullptr;
    }
};

template <class T, class U, size_t N>
auto make_pair_table(std::pair<T, U> (&&values)[N]) -> PairTable<T, U, N> {
    return PairTable<T, U, N>{
        .array = std::to_array(values),
    };
}
} // namespace

#ifdef CUTIL_NS
}
#endif
