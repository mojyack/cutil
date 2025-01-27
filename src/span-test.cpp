#include <print>
#include <string>

#include "assert.hpp"
#include "span.hpp"

auto main() -> int {
    auto str = std::string("hello");

    // str to span
    dynamic_assert(std::memcmp(str.data(), to_span(str).data(), str.size()) == 0);
    dynamic_assert(std::memcmp(str.data(), to_span<char>(str).data(), str.size()) == 0);
    dynamic_assert(std::memcmp(str.data(), to_span<std::byte>(str).data(), str.size()) == 0);

    // span to str
    auto span_char = std::span{str.data(), str.size()};
    auto span_byte = std::span{(std::byte*)str.data(), str.size()};
    dynamic_assert(std::memcmp(str.data(), from_span(span_char).data(), str.size()) == 0);
    dynamic_assert(std::memcmp(str.data(), from_span(span_byte).data(), str.size()) == 0);

    std::println("ok");
    return 0;
}
