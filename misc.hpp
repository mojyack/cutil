#pragma once
#include <fstream>
#include <vector>

#include "error.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class T = std::byte>
auto read_binary(auto path) -> Result<std::vector<T>> {
    auto ifs = std::ifstream(std::move(path));
    if(!ifs) {
        return Error("cannot open file");
    }

    ifs.seekg(0, std::ios_base::end);
    const auto cur = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, std::ios_base::beg);

    auto r = std::vector<T>((cur + sizeof(T) - 1) / sizeof(T));
    ifs.read(std::bit_cast<char*>(r.data()), cur);
    if(ifs.fail()) {
        return Error("failed to read file");
    }
    return r;
}

inline auto split(const std::string_view str, const std::string_view sep) -> std::vector<std::string_view> {
    auto ret = std::vector<std::string_view>();
    auto pos = std::string_view::size_type(0);
    while(true) {
        if(pos >= str.size()) {
            break;
        }
        const auto prev = pos;
        pos             = str.find(sep, pos);
        if(pos == std::string_view::npos) {
            if(prev != str.size()) {
                ret.emplace_back(str.substr(prev));
            }
            break;
        }

        ret.emplace_back(str.substr(prev, pos - prev));

        pos += sep.size();
    }
    return ret;
}

#ifdef CUTIL_NS
}
#endif
