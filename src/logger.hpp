#pragma once
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <mutex>
#include <source_location>
#include <string>

#include "assert.hpp"

#define CUTIL_MODULE_NAME cutil_logger_v1
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

    struct FormatString {
        const char*          format;
        std::source_location location;

        FormatString(const char* format, std::source_location location = std::source_location::current())
            : format(format),
              location(location) {}
    };

    template <Loglevel loglevel, class... Args>
    auto print(FormatString format_string, Args... args) -> void;

#pragma push_macro("print_alias")
#define print_alias(name, level)                                            \
    template <class... Args>                                                \
    auto name(FormatString format_string, Args... args) -> void {           \
        print<Loglevel::level, Args...>(std::move(format_string), args...); \
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

// impl
inline auto logger_time_base = std::chrono::system_clock::now();
inline auto stream_lock      = std::mutex();

inline auto string_to_loglevel(const std::string_view str) -> std::optional<Loglevel> {
    if(str == "error" || str == "0") {
        return Loglevel::Error;
    } else if(str == "warn" || str == "1") {
        return Loglevel::Warn;
    } else if(str == "info" || str == "2") {
        return Loglevel::Info;
    } else if(str == "debug" || str == "3") {
        return Loglevel::Debug;
    } else {
        return std::nullopt;
    }
}

template <Loglevel loglevel, class... Args>
inline auto Logger::print(FormatString format_string, Args... args) -> void {
    if(int(this->loglevel) < int(loglevel)) {
        return;
    }

    constexpr auto loglevel_str = std::array{
        "ERROR",
        "WARN",
        "INFO",
        "DEBUG",
    };
    static_assert(loglevel_str.size() == (int)Loglevel::Limit);
    constexpr auto colors = std::array{
        "\x1B[91m", // error: red
        "\x1B[93m", // warn: yellow
        "\x1B[0m",  // info: default
        "\x1B[96m", // debug: cyan
    };
    static_assert(colors.size() == (int)Loglevel::Limit);

    const auto time     = std::chrono::system_clock::now() - logger_time_base;
    const auto minutes  = std::chrono::duration_cast<std::chrono::minutes>(time);
    const auto seconds  = std::chrono::duration_cast<std::chrono::seconds>(time - minutes);
    const auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time - minutes - seconds);
    const auto time_str = build_string(minutes.count(), ":", std::setfill('0'), std::setw(2), seconds.count(), ":", std::setw(3), mseconds.count());

    const auto level = int(loglevel);
    const auto func  = impl::format_function_name(format_string.location.function_name());
    const auto file  = impl::format_file_name(format_string.location.file_name());
    const auto line  = format_string.location.line();

    // min:sec:msec [name] LEVEL func @ file.cpp:line: arguments...
    auto format = build_string(
        time_str, " ",                                     // time
        "[", name, "] ",                                   // name
        colors[level],                                     // color on
        loglevel_str[level], " ",                          // level
        func, " @ ", file, ":", std::to_string(line), " ", // location
        format_string.format,                              // user
        "\x1B[0m"                                          // color off
    );

    auto buf = std::string();
    if constexpr(sizeof...(Args) == 0) {
        buf = std::move(format);
    } else {
        // build buffer before locking stream
        const auto size = std::snprintf(NULL, 0, format.data(), args...);
        buf.resize(size);
        const auto end = std::snprintf(buf.data(), buf.size() + 1, format.data(), args...);
        buf[end]       = '\0';
    }

    // print output
    stream_lock.lock();
    std::puts(buf.data());
    stream_lock.unlock();
}

inline auto Logger::set_name_and_detect_loglevel(std::string name) -> void {
    this->name = name;

    for(auto& c : name) {
        c = (c >= 'a' && c <= 'z') ? c += ('A' - 'a') : c;
    }
    const auto env_name = name + "_LOGLEVEL";
    const auto env      = std::getenv(env_name.data());
    if(env == nullptr) {
        return;
    }
    const auto loglevel_o = string_to_loglevel(env);
    if(!loglevel_o) {
        line_warn("invalid loglevel ", env);
    } else {
        this->loglevel = *loglevel_o;
    }
}

inline Logger::Logger(std::string name) {
    set_name_and_detect_loglevel(std::move(name));
}

#include "_prologue.hpp"
