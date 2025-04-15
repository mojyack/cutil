#pragma once
#include <fstream>
#include <optional>
#include <span>
#include <vector>

inline auto read_file(const char* const path) -> std::optional<std::vector<std::byte>> {
    auto fs = std::ifstream(path, std::ios::binary);
    if(!fs) {
        return std::nullopt;
    }

    fs.seekg(0, std::ios_base::end);
    const auto cur = static_cast<size_t>(fs.tellg());
    fs.seekg(0, std::ios_base::beg);

    auto ret = std::vector<std::byte>(cur);
    fs.read(std::bit_cast<char*>(ret.data()), cur);
    fs.close();
    if(!fs) {
        return std::nullopt;
    }

    return ret;
}

inline auto write_file(const char* const path, const std::span<const std::byte> data) -> bool {
    auto fs = std::ofstream(path, std::ios::binary);
    if(!fs) {
        return false;
    }

    fs.write((const char*)data.data(), data.size());
    fs.close();
    if(!fs) {
        return false;
    }

    return true;
}
