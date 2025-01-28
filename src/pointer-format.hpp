#pragma once
#include <format>

// until C++26,P2510
template <class T, class CharT>
struct std::formatter<T*, CharT> : std::formatter<void*, CharT> {};
