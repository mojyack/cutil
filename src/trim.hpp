#pragma once
#include <string_view>

inline auto ltrim(std::string_view str) -> std::string_view {
    while(!str.empty() && std::isspace(str[0])) {
        str.remove_prefix(1);
    }
    return str;
}

inline auto rtrim(std::string_view str) -> std::string_view {
    while(!str.empty() && std::isspace(str.back())) {
        str.remove_suffix(1);
    }
    return str;
}

inline auto trim(std::string_view str) -> std::string_view {
    return rtrim(ltrim(str));
}
