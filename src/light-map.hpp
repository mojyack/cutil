#pragma once
#include <vector>

template <class K, class V>
struct LightMap {
    using E = std::pair<K, V>;
    using I = std::vector<E>::iterator;
    std::vector<E> data;

    auto find(const auto& key) -> I {
        auto i = begin();
        for(; i < end(); i += 1) {
            if(i->first == key) {
                break;
            }
        }
        return i;
    }

    auto contains(const auto& key) -> bool {
        return find(key) != end();
    }

    auto erase(I i) -> I {
        return data.erase(i);
    }

    auto erase(const auto& key) -> bool {
        auto i = find(key);
        if(i != end()) {
            data.erase(i);
            return true;
        } else {
            return false;
        }
    }

    auto begin() -> I {
        return data.begin();
    }

    auto end() -> I {
        return data.end();
    }

    auto operator[](const auto& key) -> V& {
        if(const auto i = find(key); i != end()) {
            return i->second;
        } else {
            return data.emplace_back(E{key, {}}).second;
        }
    }
};
