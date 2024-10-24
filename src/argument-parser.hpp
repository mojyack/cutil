#pragma once
#include <string_view>
#include <vector>

#include "charconv.hpp"
#include "print.hpp"
#include "variant.hpp"

#define CUTIL_MODULE_NAME cutil_argument_parser_v2
#include "_prologue.hpp"

namespace args {
using CStr = const char*;

// from string
template <class T>
inline auto from_string(CStr str) -> std::optional<T>;

template <>
inline auto from_string<bool>(CStr) -> std::optional<bool> {
    return std::nullopt;
}

template <class T>
    requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
inline auto from_string(CStr str) -> std::optional<T> {
    return from_chars<T>(str);
}

template <>
inline auto from_string<double>(CStr str) -> std::optional<double> {
    errno        = 0;
    const auto v = std::strtod(std::string(str).data(), NULL);
    return errno == 0 ? std::optional(v) : std::nullopt;
}

template <>
inline auto from_string<CStr>(CStr str) -> std::optional<CStr> {
    return str;
}

// to string
template <class T>
auto to_string(const T& data) -> std::string;

template <>
inline auto to_string<bool>(const bool&) -> std::string {
    return "";
}

template <class T>
    requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
auto to_string(const T& data) -> std::string {
    return std::to_string(data);
}

template <>
inline auto to_string<double>(const double& data) -> std::string {
    return std::to_string(data);
}

template <>
inline auto to_string<CStr>(const CStr& data) -> std::string {
    return data;
}

enum class State {
    Uninitialized,
    Initialized,
    DefaultValue,
};

struct ArgumentSpec {
    std::string_view value_desc        = "";
    std::string_view arg_desc          = "";
    State            state             = State::Uninitialized;
    bool             invert_flag_value = false;
    bool             no_error_check    = false;
};

using Keys = std::vector<std::string_view>;

#pragma push_macro("bail")
#pragma push_macro("assert")
#undef bail
#undef assert
#define bail(...)      \
    warn(__VA_ARGS__); \
    return false;

#define assert(cond, ...) \
    if(!(cond)) {         \
        bail(__VA_ARGS__) \
    }

template <class... Ts>
class GenericParser {
  private:
    template <class T>
    struct PtrInitPair {
        T* ptr;
        T  init;
    };

    using Pair = Variant<PtrInitPair<Ts>...>;

    template <class T>
    constexpr static size_t index_of = Pair::template index_of<PtrInitPair<T>>;

    template <class T>
    auto as_pair(Pair& pair) -> PtrInitPair<T>& {
        return pair.template as<PtrInitPair<T>>();
    }

    struct Argument {
        Pair         pair;
        ArgumentSpec spec;
        bool         found = false;
    };
    std::vector<std::pair<Keys, Argument>> keyword_args;
    std::vector<Argument>                  args;

  public:
    template <class T>
    auto arg(T* data, ArgumentSpec spec) -> void {
        args.push_back({
            .pair = Pair::template create<PtrInitPair<T>>(data, *data),
            .spec = spec,
        });
    }

    template <class T>
    auto kwarg(T* data, Keys keys, ArgumentSpec spec) -> void {
        keyword_args.push_back({
            std::move(keys),
            {
                .pair = Pair::template create<PtrInitPair<T>>(data, *data),
                .spec = spec,
            },
        });
    }

    auto get_help() const -> std::string {
        auto ret = std::string();
        if(!keyword_args.empty()) {
            ret += "(options)... ";
        }
        for(const auto& entry : args) {
            ret += entry.spec.value_desc;
            ret += " ";
        }
        ret += "\noptions:\n";
        auto maxlen = size_t(0);
        auto lines  = std::vector<std::string>();
        for(const auto& [keys, entry] : keyword_args) {
            auto& line = lines.emplace_back();
            line += "  ";
            for(const auto key : keys) {
                line += key;
                line += ",";
            }
            line.back() = ' ';
            if(entry.pair.get_index() != index_of<bool>) {
                line += entry.spec.value_desc;
                line += " ";
            }
            maxlen = std::max(maxlen, line.size());
        }
        for(auto i = 0u; i < keyword_args.size(); i += 1) {
            const auto& entry = keyword_args[i].second;
            const auto& line  = lines[i];
            ret += line;
            ret += std::string(maxlen - line.size() + 2, ' ');
            if(entry.spec.state == State::Uninitialized) {
                ret += "required: ";
            }
            ret += entry.spec.arg_desc;
            if(entry.spec.state == State::DefaultValue) {
                ret += "(default=";
                ret += entry.pair.apply([](auto& pair) { return to_string(pair.init); }).value();
                ret += ")";
            }
            ret += "\n";
        }
        return ret;
    }

    auto parse(const int argc, const char* const* const argv) -> bool {
        auto parse_data = [](Argument& entry, CStr str) -> bool {
            const auto ret = entry.pair.apply([str](auto& pair) {
                using T = decltype(pair.init);

                auto value = from_string<T>(str);
                assert(value, "failed to parse argument ", str);
                *pair.ptr = *value;
                return true;
            });
            return ret && *ret;
        };

        auto skip_error_check = false;
        auto index            = 1;
    loop:
        if(index >= argc) {
            goto check;
        }

        for(auto& [keys, entry] : keyword_args) {
            if(entry.found || std::ranges::find(keys, argv[index]) == keys.end()) {
                continue;
            }
            switch(entry.pair.get_index()) {
            case index_of<bool>:
                *as_pair<bool>(entry.pair).ptr = entry.spec.invert_flag_value ? false : true;
                break;
            default:
                index += 1;
                assert(index < argc, "no following argument to ", keys[0]);
                assert(parse_data(entry, argv[index]));
                break;
            }
            entry.found = true;
            skip_error_check |= entry.spec.no_error_check;
            goto next;
        }

        for(auto& entry : args) {
            if(entry.found) {
                continue;
            }
            switch(entry.pair.get_index()) {
            case index_of<bool>:
                bail("flag cannot be a positional argument");
            default:
                assert(parse_data(entry, argv[index]));
                break;
            }
            entry.found = true;
            goto next;
        }
        bail("unhandled argument ", argv[index]);

    next:
        index += 1;
        goto loop;

    check:
        if(skip_error_check) {
            return true;
        }
        for(auto& [keys, entry] : keyword_args) {
            assert(entry.found || entry.spec.state != State::Uninitialized, "required argument ", keys[0], " is missing");
        }
        for(auto& entry : args) {
            assert(entry.found || entry.spec.state != State::Uninitialized, "required positional argument is missing");
        }

        return true;
    }
};

template <class... Ts>
using Parser = GenericParser<bool, int, double, CStr, Ts...>;

#pragma pop_macro("assert")
#pragma pop_macro("bail")

} // namespace args

#include "_prologue.hpp"
