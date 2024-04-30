#pragma once
#include <iostream>
#include <sstream>
#include <variant>

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

template <class T, class E>
class Result {
  private:
    std::variant<T, E> data;

  public:
    auto as_value() -> T& {
        return std::get<T>(data);
    }

    auto as_value() const -> const T& {
        return std::get<T>(data);
    }

    auto as_error() const -> E {
        return std::get<E>(data);
    }

    auto unwrap() -> T& {
        if(!std::holds_alternative<T>(data)) {
            throw std::runtime_error(as_error().cstr());
        }
        return as_value();
    }

    auto unwrap() const -> const T& {
        if(!std::holds_alternative<T>(data)) {
            throw std::runtime_error(as_error().cstr());
        }
        return as_value();
    }

    operator bool() const {
        return std::holds_alternative<T>(data);
    }

    Result(T data) : data(std::move(data)) {}

    Result(const E error) : data(error) {}
};

#ifdef CUTIL_NS
}
#endif
