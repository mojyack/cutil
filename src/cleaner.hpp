#pragma once
#include <type_traits>

#define CUTIL_MODULE_NAME cutil_cleaner_v1
#include "_prologue.hpp"

template <class T>
    requires std::is_invocable_v<T>
struct Cleaner {
    T func;

    ~Cleaner() {
        func();
    }
};

#include "_prologue.hpp"
