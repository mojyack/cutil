#define CUTIL_NS
#include "assert.hpp"
#include "variant.hpp"

namespace {
auto variant_test() -> void {
    {
        using V = Variant<int, float>;

        dynamic_assert(!V().is_valid());

        auto v = V::create<int>(1);
        auto u = V::create<float>(1.0);
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
        struct Count {
            int constructor      = 0;
            int copy_constructor = 0;
            int move_constructor = 0;
            int copy_assign      = 0;
            int move_assign      = 0;
            int destructor       = 0;
        };

        struct Noisy {
            Count* count;
            auto   operator=(Noisy&&) {
                count->move_assign += 1;
            }

            auto operator=(Noisy&) {
                count->copy_assign += 1;
            }

            Noisy(Count* const count) : count(count) {
                count->constructor += 1;
            }

            Noisy(Noisy&& o) {
                count = o.count;
                count->move_constructor += 1;
            }

            Noisy(Noisy& o) {
                count = o.count;
                count->copy_constructor += 1;
            }

            ~Noisy() {
                count->destructor += 1;
            }
        };

        using V = Variant<Noisy>;

        // constructor/destructor test
        auto v  = V();
        auto cv = Count();

        v.emplace<Noisy>(&cv);
        dynamic_assert(cv.constructor == 1);

        v.reset();
        dynamic_assert(cv.destructor == 1);

        // copy/move constructor test of contents
        cv      = Count();
        auto u  = V();
        auto cu = Count();

        v.emplace<Noisy>(&cu);

        u = v;
        dynamic_assert(cu.copy_constructor == 1 && v.is_valid());

        u.reset();
        u = std::move(v);
        dynamic_assert(cu.move_constructor == 1 && !v.is_valid());

        // copy/move assignment test
        cv = Count();
        cu = Count();
        v.emplace<Noisy>(&cv);
        u.emplace<Noisy>(&cu);

        u = v;
        dynamic_assert(cu.copy_assign == 1 && v.is_valid());

        u = std::move(v);
        dynamic_assert(cu.move_assign == 1 && !v.is_valid());

        // constructor test of variant
        cv = Count();
        cu = Count();

        auto x = u;
        dynamic_assert(cu.copy_constructor == 1 && u.is_valid());
        auto y = std::move(u);
        dynamic_assert(cu.move_constructor == 1 && !u.is_valid());
    }
}
} // namespace

auto main() -> int {
    variant_test();
    print("pass");
    return 0;
}
