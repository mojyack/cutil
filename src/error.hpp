#pragma once
#include <string>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#else
namespace {
#endif

class StringError {
  private:
    std::string what;

  public:
    auto cstr() const -> const char* {
        return what.data();
    }

    operator bool() const {
        return !what.empty();
    }

    StringError() = default;
    StringError(std::string_view what) : what(what) {}
};
}
