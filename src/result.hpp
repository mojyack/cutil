#pragma once
#include <variant>

#include "assert.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#else
namespace {
#endif

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
        dynamic_assert(std::holds_alternative<T>(data));
        return as_value();
    }

    auto unwrap() const -> const T& {
        dynamic_assert(std::holds_alternative<T>(data));
        return as_value();
    }

    operator bool() const {
        return std::holds_alternative<T>(data);
    }

    Result(T data) : data(std::move(data)) {}

    Result(const E error) : data(error) {}
};
}
