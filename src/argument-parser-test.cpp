#include "argument-parser.hpp"
#include "misc.hpp"

struct Pos {
    int x;
    int y;
};

namespace args {
namespace {
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
    return build_string(data.x, ",", data.y);
}
} // namespace
} // namespace args

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

auto test1() -> void {
    struct Args {
        int         pint;
        int         kint;
        double      pdouble;
        double      kdouble;
        const char* pstr;
        const char* kstr;
        bool        pflag;
        bool        kflag;
        Pos         ppos;
        Pos         kpos;
    };
    auto parser = args::Parser<Pos>();
    auto args   = Args();
    parser.arg(&args.pint, {.value_desc = "INT", .arg_desc = "positional interger value"});
    parser.kwarg(&args.kint, {"-n", "--num"}, {.value_desc = "INT", .arg_desc = "positional interger value"});
    parser.arg(&args.pdouble, {.value_desc = "FLOAT", .arg_desc = "positional float value"});
    parser.kwarg(&args.kdouble, {"-f", "--float"}, {.value_desc = "FLOAT", .arg_desc = "positional float value"});
    parser.arg(&args.pstr, {.value_desc = "STR", .arg_desc = "positional string value"});
    parser.kwarg(&args.kstr, {"-s", "--string"}, {.value_desc = "STR", .arg_desc = "positional string value"});
    parser.kwarg(&args.kflag, {"-b", "--bool"}, {.value_desc = "FLAG", .arg_desc = "positional boolean value"});
    parser.arg(&args.ppos, {.value_desc = "X,Y", .arg_desc = "custom value type"});
    parser.kwarg(&args.kpos, {"-p", "--pos"}, {.value_desc = "FLOAT", .arg_desc = "custom value type"});
    print("usage: test ", parser.get_help());
    print("parse: ", parse(parser, "test -n 2 -f 2.0 -s hello -b 1 -p 100,200 1.0 world 300,400") ? "ok" : "error");
    if(args.pint != 1 || args.kint != 2 ||
       args.pdouble != 1.0 || args.kdouble != 2.0 ||
       std::string_view(args.kstr) != "hello" || std::string_view(args.pstr) != "world" ||
       args.kflag != true ||
       args.ppos.x != 300 || args.ppos.y != 400 || args.kpos.x != 100 || args.kpos.y != 200) {
        print("args: error");
    } else {
        print("args: ok");
    }
    print("pint: ", args.pint);
    print("kint: ", args.kint);
    print("pdouble: ", args.pdouble);
    print("kdouble: ", args.kdouble);
    print("pstr: ", args.pstr);
    print("kstr: ", args.kstr);
    // print("pflag: ", args.pflag);
    print("kflag: ", args.kflag);
    print("ppos: ", args.ppos.x, ",", args.ppos.y);
    print("kpos: ", args.kpos.x, ",", args.kpos.y);
}

auto test2() -> void {
    struct Args {
        bool flag;
        bool invert;
    };
    auto parser = args::Parser();
    auto args   = Args();
    parser.kwarg(&args.flag, {"-1"}, {.arg_desc = "normal"});
    parser.kwarg(&args.invert, {"-2"}, {.arg_desc = "invert", .invert_flag_value = true});
    print("usage: test ", parser.get_help());
    print("parse: ", parse(parser, "test -1 -2") ? "ok" : "error");
    print("args:", args.flag && !args.invert ? "ok" : "error");
}

auto test3() -> void {
    struct Args {
        int num1 = 8086;
        int num2 = -1;
    };
    auto parser = args::Parser();
    auto args   = Args();
    parser.kwarg(&args.num1, {"-1"}, {.value_desc = "INT", .arg_desc = "with default value", .state = args::State::DefaultValue});
    parser.kwarg(&args.num2, {"-2"}, {.value_desc = "INT", .arg_desc = "disabled", .state = args::State::Initialized});
    print("usage: test ", parser.get_help());
    print("parse: ", parse(parser, "test -1 8080 -2 1") ? "ok" : "error");
    print("args:", args.num1 == 8080 && args.num2 == 1 ? "ok" : "error");
}

auto test4() -> void {
    struct Args {
        int8_t  i8;
        uint8_t u8;
        ssize_t i64;
        size_t  u64;
    };
    auto parser = args::Parser<int8_t, uint8_t, ssize_t, size_t>();
    auto args   = Args();
    parser.arg(&args.i8, {.value_desc = "INT", .arg_desc = "i8"});
    parser.arg(&args.u8, {.value_desc = "INT", .arg_desc = "u8"});
    parser.arg(&args.i64, {.value_desc = "INT", .arg_desc = "i64"});
    parser.arg(&args.u64, {.value_desc = "INT", .arg_desc = "u64"});
    print("usage: test ", parser.get_help());
    print("parse: ", parse(parser, "test -1 2 -3 4") ? "ok" : "error");
    print("args:", args.i8 == -1 && args.u8 == 2 && args.i64 == -3 && args.u64 == 4 ? "ok" : "error");
}

auto test5() -> void {
    struct Args {
        int8_t  i8;
        uint8_t u8;
    };
    auto parser = args::Parser<int8_t, uint8_t, ssize_t, size_t>();
    auto args   = Args();
    parser.arg(&args.i8, {.value_desc = "INT", .arg_desc = "i8"});
    parser.arg(&args.u8, {.value_desc = "INT", .arg_desc = "u8"});
    print("parse: ", !parse(parser, "test 1 -2") ? "ok" : "error");
}

auto main() -> int {
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
