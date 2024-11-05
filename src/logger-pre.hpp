#pragma once
#include <source_location>
#include <string>

#define CUTIL_MODULE_NAME cutil_logger_v2
#include "_prologue.hpp"

enum class Loglevel : uint8_t {
    Error = 0,
    Warn,
    Info,
    Debug,

    Limit,
};

inline auto string_to_loglevel(std::string_view str) -> std::optional<Loglevel>;

struct Logger {
    std::string name;
    Loglevel    loglevel = Loglevel::Info;

    struct FirstArgument {
        const char*          value;
        std::source_location location;

        FirstArgument(const char* value, std::source_location location = std::source_location::current())
            : value(value),
              location(location) {}
    };

    template <Loglevel loglevel, class... Args>
    auto print(FirstArgument format_string, const Args&... args) const -> void;

#pragma push_macro("print_alias")
#define print_alias(name, level)                                      \
    template <class... Args>                                          \
    auto name(FirstArgument arg, const Args&... args) const -> void { \
        print<Loglevel::level, Args...>(std::move(arg), args...);     \
    }

    print_alias(error, Error);
    print_alias(warn, Warn);
    print_alias(info, Info);
    print_alias(debug, Debug);
#pragma pop_macro("print_alias")

    auto set_name_and_detect_loglevel(std::string name) -> void;

    Logger() = default;
    Logger(std::string name);
};

#include "_prologue.hpp"
