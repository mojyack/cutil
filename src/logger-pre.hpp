#pragma once
#include <optional>
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

    auto set_name_and_detect_loglevel(std::string name) -> void;

    Logger() = default;
    Logger(std::string name);
};

#include "_prologue.hpp"
