#pragma once
#include <string>

#define CUTIL_MODULE_NAME cutil_error_v1
#include "_prologue.hpp"

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

#include "_prologue.hpp"
