#include <cstdint>

#include "argument-parser.hpp"
#include "split.hpp"

struct Pos {
    int x;
    int y;
};

namespace args {
template <>
auto from_string<Pos>(const char* str) -> std::optional<Pos> {
    auto elms = split(str, ",");
    if(elms.size() != 2) {
        return std::nullopt;
    }
    auto x = from_chars<int>(elms[0]);
    auto y = from_chars<int>(elms[1]);
    if(!x || !y) {
        return std::nullopt;
    }
    return Pos{*x, *y};
}

template <>
auto to_string<Pos>(const Pos& data) -> std::string {
    return std::format("{},{}", data.x, data.y);
}
} // namespace args

namespace test {
auto parse(auto& parser, const char* const str) -> bool {
    auto strs = std::vector<std::string>();
    auto argv = std::vector<const char*>();
    for(const auto elm : split(str, " ")) {
        strs.emplace_back(elm);
    }
    for(const auto& str : strs) {
        argv.emplace_back(str.data());
    }
    return parser.parse(argv.size(), argv.data());
}

#define assert(cond)                                              \
    if(!(cond)) {                                                 \
        std::println(stderr, "assertion failed at {}", __LINE__); \
        return false;                                             \
    }

auto all_type_test() -> bool {
    auto pint    = int();
    auto kint    = int();
    auto pdouble = double();
    auto kdouble = double();
    auto pstr    = (const char*)(nullptr);
    auto kstr    = (const char*)(nullptr);
    auto pflag   = bool();
    auto kflag   = bool();
    auto ppos    = Pos();
    auto kpos    = Pos();

    auto parser = args::Parser<Pos>();
    parser.arg(&pint, "INT", "positional interger value");
    parser.kwarg(&kint, {"-n", "--num"}, "INT", "keyword interger value");
    parser.arg(&pdouble, "FLOAT", "positional float value");
    parser.kwarg(&kdouble, {"-f", "--float"}, "FLOAT", "keyword float value");
    parser.arg(&pstr, "STR", "positional string value");
    parser.kwarg(&kstr, {"-s", "--string"}, "STR", "keyword string value");
    parser.arg(&pflag, "BOOL", "positional boolean value");
    parser.kwarg(&kflag, {"-b", "--bool"}, "BOOL", "keyword boolean value");
    parser.arg(&ppos, "X,Y", "positional custom value");
    parser.kwarg(&kpos, {"-p", "--pos"}, "FLOAT", "keyword custom value");
    std::println("usage: test {}", parser.get_help());
    std::println("parse: {}", parse(parser, "test -n 2 -f 2.0 -s hello -b true -p 100,200 1 1.0 world false 300,400") ? "ok" : "error");
    assert(pint == 1);
    assert(kint == 2);
    assert(pdouble == 1.0);
    assert(kdouble == 2.0);
    assert(std::string_view(kstr) == "hello");
    assert(std::string_view(pstr) == "world");
    assert(pflag == false);
    assert(kflag == true);
    assert(ppos.x == 300 || ppos.y == 400 || kpos.x == 100 || kpos.y == 200);
    return true;
}

auto flag_test() -> bool {
    auto flag   = bool();
    auto invert = bool();

    auto parser = args::Parser<>();
    parser.kwflag(&flag, {"-1"}, "normal");
    parser.kwflag(&invert, {"-2"}, "invert", {.invert_flag_value = true});
    std::println("usage: test {}", parser.get_help());
    assert(parse(parser, "test -1 -2"));
    assert(flag && !invert);
    return true;
}

auto state_test() -> bool {
    auto num = 8086;
    // default value
    {
        auto parser = args::Parser<>();
        parser.kwarg(&num, {"-n"}, "INT", "number", {.state = args::State::DefaultValue});
        std::println("usage: test {}", parser.get_help());
        assert(parse(parser, "test"));
        assert(num == 8086);
    }
    // initialized
    {
        auto parser = args::Parser<>();
        parser.kwarg(&num, {"-n"}, "INT", "number", {.state = args::State::Initialized});
        std::println("usage: test {}", parser.get_help());
        assert(parse(parser, "test"));
        assert(num == 8086);
    }
    // required
    {
        auto parser = args::Parser<>();
        parser.kwarg(&num, {"-n"}, "INT", "number", {.state = args::State::Uninitialized});
        std::println("usage: test {}", parser.get_help());
        assert(!parse(parser, "test"));
        assert(parse(parser, "test -n 1"));
        assert(num == 1);
    }
    return true;
}

auto minus_int_test() -> bool {
    auto i8  = int8_t();
    auto u8  = uint8_t();
    auto i64 = ssize_t();
    auto u64 = size_t();

    auto parser = args::Parser<int8_t, uint8_t, ssize_t, size_t>();
    parser.arg(&i8, "INT", "i8");
    parser.arg(&u8, "INT", "u8");
    parser.arg(&i64, "INT", "i64");
    parser.arg(&u64, "INT", "u64");
    std::println("usage: test {}", parser.get_help());
    assert(parse(parser, "test -1 2 -3 4"));
    assert(i8 == -1 && u8 == 2 && i64 == -3 && u64 == 4);
    return true;
}

auto out_of_range_test() -> bool {
    auto i8 = int8_t();
    auto u8 = uint8_t();

    auto parser = args::Parser<int8_t, uint8_t, ssize_t, size_t>();
    parser.arg(&i8, "INT", "i8");
    parser.arg(&u8, "INT", "u8");
    assert(!parse(parser, "test 1 -2"));
    return true;
}

auto no_error_check_test() -> bool {
    auto required = bool();
    auto help     = false;

    auto parser = args::Parser<>();
    parser.arg(&required, "BOOL", "required");
    parser.kwflag(&help, {"-h", "--help"}, "help", {.no_error_check = true});
    assert(parse(parser, "test -h"));
    return true;
}

auto unhandled_test() -> bool {
    auto kwarg = int();
    auto arg   = int();

    {
        auto parser = args::Parser<>();
        parser.kwarg(&kwarg, {"-k"}, "INT", "number", {.state = args::State::Uninitialized});
        parser.arg(&arg, "INT", "number", {.state = args::State::Uninitialized});

        const auto argv = std::vector{"test", "-k", "1", "2"};

        auto index = int();
        assert(parser.parse(argv.size(), argv.data(), &index));
        assert(index == int(argv.size()));
    }
    {
        auto parser = args::Parser<>();
        parser.kwarg(&kwarg, {"-k"}, "INT", "number", {.state = args::State::Uninitialized});
        parser.arg(&arg, "INT", "number", {.state = args::State::Uninitialized});

        const auto argv = std::vector{"test", "-k", "1", "2", "a", "b", "c"};

        auto index = int();
        assert(parser.parse(argv.size(), argv.data(), &index));
        std::println("index={} {}", index, (void*)&index);
        assert(argv[index + 0][0] == 'a');
        assert(argv[index + 1][0] == 'b');
        assert(argv[index + 2][0] == 'c');
    }
    return true;
}
} // namespace test

auto main() -> int {
    auto ret = true;
    for(auto test : {
            test::all_type_test,
            test::flag_test,
            test::state_test,
            test::minus_int_test,
            test::out_of_range_test,
            test::no_error_check_test,
            test::unhandled_test,
        }) {
        if(!test()) {
            std::println("test failed");
            ret = false;
        }
    }
    std::println("result: {}", ret ? "ok" : "error");
    return ret ? 0 : 1;
}
