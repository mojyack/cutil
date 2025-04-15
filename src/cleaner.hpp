#pragma once
#include <type_traits>

template <class T>
    requires std::is_invocable_v<T>
struct Cleaner {
    T func;

    ~Cleaner() {
        func();
    }
};
