#include "assert.hpp"
#include "variant.hpp"

namespace {
auto variant_test() -> void {
    {
        using V = Variant<int, float>;

        dynamic_assert(!V().is_valid());

        auto v = V(Tag<int>(), 1);
        auto u = V(Tag<float>(), 1.0);
        dynamic_assert(*v.get<int>() == 1);
        dynamic_assert(*u.get<float>() == 1.0);

        auto e = V();
        v      = e;
        dynamic_assert(!v.is_valid());

        v = std::move(u);
        dynamic_assert(*v.get<float>() == 1.0);
        dynamic_assert(!u.is_valid());
    }
    {
        static auto constructor      = 0;
        static auto move_constructor = 0;
        static auto copy_assign      = 0;
        static auto move_assign      = 0;
        static auto copy_constructor = 0;
        static auto destructor       = 0;
        struct Noisy {
            auto operator=(Noisy&&) {
                move_assign += 1;
            }

            auto operator=(Noisy&) {
                copy_assign += 1;
            }

            Noisy() {
                constructor += 1;
            }

            Noisy(Noisy&&) {
                move_constructor += 1;
            }

            Noisy(Noisy&) {
                copy_constructor += 1;
            }

            ~Noisy() {
                destructor += 1;
            }
        };

        using V = Variant<Noisy>;

        auto v = V();
        dynamic_assert(constructor == 0);

        v.emplace<Noisy>();
        dynamic_assert(constructor == 1);

        v.reset();
        dynamic_assert(destructor == 1);

        auto u = V();
        v.emplace<Noisy>();

        u = v;
        dynamic_assert(copy_constructor == 1 && v.is_valid());

        u.reset();
        u = std::move(v);
        dynamic_assert(move_constructor == 1 && !v.is_valid());

        v.emplace<Noisy>();
        u.emplace<Noisy>();

        u = v;
        dynamic_assert(copy_assign == 1 && v.is_valid());

        u = std::move(v);
        dynamic_assert(move_assign == 1 && !v.is_valid());
    }
}
} // namespace

auto main() -> int {
    variant_test();
    print("pass");
    return 0;
}
