#pragma once
#include <string_view>

#define CUTIL_MODULE_NAME cutil_trim_v1
#include "_prologue.hpp"

auto ltrim(std::string_view str) -> std::string_view {
    while(!str.empty() && std::isspace(str[0])) {
        str.remove_prefix(1);
    }
    return str;
}

auto rtrim(std::string_view str) -> std::string_view {
    while(!str.empty() && std::isspace(str.back())) {
        str.remove_suffix(1);
    }
    return str;
}

auto trim(std::string_view str) -> std::string_view {
    return rtrim(ltrim(str));
}

#include "_prologue.hpp"
