#pragma once
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <mutex>

#include "assert.hpp"
#include "location-print.hpp"
#include "logger-pre.hpp"

#define CUTIL_MODULE_NAME cutil_logger_v2
#include "_prologue.hpp"

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

template <comptime::String filename, comptime::String function, size_t line, Loglevel loglevel, class... Args>
inline auto logger_print(const Logger& logger, Args&&... args) -> void {
    if(int(logger.loglevel) < int(loglevel)) {
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

    const auto     level          = int(loglevel);
    constexpr auto short_filename = cutil_impl::format_file_name<filename>();
    constexpr auto short_function = cutil_impl::format_function_name<function>();

    // min:sec:msec [name] LEVEL func @ file.cpp:line arguments...
    auto string = build_string(
        time_str, " ",                                                     // time
        "[", logger.name, "] ",                                            // name
        colors[level],                                                     // color on
        loglevel_str[level], " ",                                          // level
        short_function.str(), " @ ", short_filename.str(), ":", line, " ", // location
        args...,                                                           // user
        "\x1B[0m"                                                          // color off
    );

    // print output
    stream_lock.lock();
    (level <= int(Loglevel::Warn) ? std::cerr : std::cout) << string << std::endl;
    stream_lock.unlock();
}

inline auto Logger::set_name_and_detect_loglevel(std::string name) -> void {
    this->name = name;

    for(auto& c : name) {
        c = (c >= 'a' && c <= 'z') ? c + ('A' - 'a') : c;
    }
    const auto env_name = name + "_LOGLEVEL";
    const auto env      = std::getenv(env_name.data());
    if(env == nullptr) {
        return;
    }
    const auto loglevel_o = string_to_loglevel(env);
    if(!loglevel_o) {
        warn("invalid loglevel ", env);
    } else {
        this->loglevel = *loglevel_o;
    }
}

inline Logger::Logger(std::string name) {
    set_name_and_detect_loglevel(std::move(name));
}

#include "_prologue.hpp"
