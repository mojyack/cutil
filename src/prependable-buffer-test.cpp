#include <algorithm>
#include <print>

#include "concat.hpp"
#include "hexdump.hpp"
#include "prependable-buffer.hpp"

#define ensure(cond)                           \
    if(!(cond)) {                              \
        std::println("error at {}", __LINE__); \
        dump_hex(buf.storage);                 \
        return false;                          \
    }

auto append_prepend_test() -> bool {
    auto buf      = PrependableBuffer();
    auto span     = std::span<std::byte>();
    auto expected = std::vector<std::byte>();

    ensure(buf.size() == 0);

    // normal append: append 0x01 @ 8
    span = buf.enlarge(8);
    // ---- ---- ---- ---- **** 40 bytes
    //                     ^ head(32)
    std::ranges::fill(span, std::byte(1));

    ensure(buf.size() == 8);
    expected = concat(expected, std::vector<std::byte>(8, std::byte(1)));
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    // normal append: append 0x02 @ 8
    span = buf.enlarge(8);
    // ---- ---- ---- ---- **** ---- 48 bytes
    //                     ^ head(32)
    std::ranges::fill(span, std::byte(2));

    ensure(buf.size() == 8 + 8);
    expected = concat(expected, std::vector<std::byte>(8, std::byte(2)));
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    const auto prev_data = buf.body().data();

    // normal prepend: prepend 0x03 @ 8
    span = buf.enlarge_forward(8);
    // ---- ---- ---- **** ---- ---- 48 bytes
    //                ^ head(24)
    std::ranges::fill(span, std::byte(3));

    ensure(buf.size() == 8 + 8 + 8);
    ensure(buf.body().data() == prev_data - 8);
    expected = concat(std::vector<std::byte>(8, std::byte(3)), expected);
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    // over sized prepend: prepend 0x04 @ 32
    span = buf.enlarge_forward(32);
    // ---- ---- ---- ---- **** **** **** **** ---- ---- ---- 88 bytes
    //                     ^ head(32)
    std::ranges::fill(span, std::byte(4));

    ensure(buf.size() == 32 + 8 + 8 + 8);
    expected = concat(std::vector<std::byte>(32, std::byte(4)), expected);
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    // shrink
    buf.resize(8);
    ensure(buf.size() == 8);
    expected.resize(8);
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    return true;
}

auto prepend_first_test() -> bool {
    auto buf      = PrependableBuffer();
    auto span     = std::span<std::byte>();
    auto expected = std::vector<std::byte>();

    // prepend 0x01 @ 8 to empty buffer
    span = buf.enlarge_forward(8);
    // ---- ---- ---- ---- **** 40 bytes
    //                     ^ head(32)
    std::ranges::fill(span, std::byte(1));

    ensure(buf.size() == 8);
    expected = concat(std::vector<std::byte>(8, std::byte(1)), expected);
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    const auto prev_data = buf.body().data();

    // prepend more
    span = buf.enlarge_forward(8);
    // ---- ---- ---- **** ---- 40 bytes
    //                ^ head(24)
    std::ranges::fill(span, std::byte(2));

    ensure(buf.size() == 8 + 8);
    ensure(buf.body().data() == prev_data - 8);
    expected = concat(std::vector<std::byte>(8, std::byte(2)), expected);
    ensure(std::memcmp(buf.body().data(), expected.data(), buf.size()) == 0);

    return true;
}

auto append_object_test() -> bool {
    auto buf = PrependableBuffer();

    struct Object {
        int  a;
        char b;
    };
    auto object = Object();
    buf.append_object(object);
    ensure(std::memcmp(buf.body().data(), &object, sizeof(Object)) == 0);

    return true;
}

auto append_array_test() -> bool {
    auto buf = PrependableBuffer();

    auto array = std::array<std::byte, 8>();
    buf.append_array(array);
    ensure(std::memcmp(buf.body().data(), array.data(), array.size()) == 0);

    return true;
}

auto main() -> int {
    if(append_prepend_test() && prepend_first_test() && append_object_test() && append_array_test()) {
        std::println("pass");
        return 0;
    } else {
        return -1;
    }
}
