#include <fstream>
#include <vector>

#include "error.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class T = std::byte>
auto read_binary(std::string_view path) -> Result<std::vector<T>> {
    auto ifs = std::ifstream(path);
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


#ifdef CUTIL_NS
}
#endif
