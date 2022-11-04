#pragma once
#include <exception>
#include <iostream>
#include <sstream>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    auto ss = std::stringstream();
    (ss << ... << args) << std::endl;
    throw std::runtime_error(ss.str());
}

template <class... Args>
auto warn(Args... args) -> void {
    (std::cerr << ... << args) << std::endl;
}

template <class... Args>
auto print(Args... args) -> void {
    (std::cout << ... << args) << std::endl;
}

template <class... Args>
auto build_string(Args... args) -> std::string {
    auto ss = std::stringstream();
    (ss << ... << args);
    return ss.str();
}

template <class... Args>
auto dynamic_assert(const bool cond, Args... args) -> void {
    if(!cond) {
        panic(args...);
    }
}

class Error {
  private:
    std::string what;

  public:
    auto cstr() -> const char* {
        return what.data();
    }

    Error() = default;
    Error(std::string_view what) : what(what) {}
};

template <class T>
class Result {
  private:
    std::variant<T, Error> data;

  public:
    auto as_value() -> T& {
        return std::get<T>(data);
    }

    auto as_value() const -> const T& {
        return std::get<T>(data);
    }

    auto as_error() const -> Error {
        return std::get<Error>(data);
    }

    operator bool() const {
        return std::holds_alternative<T>(data);
    }

    Result(T&& data) : data(std::move(data)) {}

    Result(const Error error = Error()) : data(error) {}
};

#ifdef CUTIL_NS
}
#endif
