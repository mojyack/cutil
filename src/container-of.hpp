#pragma once
#include <bit>

namespace CUTIL_NS {
template <class Container, class Member>
auto container_of(Member* const ptr, const Member Container::*member) -> Container* {
    struct OffsetOf {
        static auto offset_of(const Member Container::*member) -> size_t {
            return size_t(&(std::bit_cast<Container*>(nullptr)->*member));
        }
    };

    return std::bit_cast<Container*>(std::bit_cast<std::byte*>(ptr) - OffsetOf::offset_of(member));
}
} // namespace CUTIL_NS
