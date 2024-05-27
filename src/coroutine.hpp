#pragma once
#include <coroutine>
#include <utility>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

template <class T>
struct CoGenerator {
    struct promise_type {
        T data;

        auto get_return_object() -> CoGenerator {
            return CoGenerator(*this);
        }

        auto initial_suspend() -> std::suspend_always {
            return {};
        }

        auto final_suspend() noexcept -> std::suspend_always {
            return {};
        }

        auto yield_value(T data) -> std::suspend_always {
            this->data = std::move(data);
            return {};
        }

        // R == void
        // auto return_void() -> void {
        // }

        auto return_value(T data) -> void {
            this->data = std::move(data);
        }

        auto unhandled_exception() -> void {
            exit(0);
        }
    };

    std::coroutine_handle<promise_type> handle;

    auto operator=(CoGenerator&) -> CoGenerator& = delete;

    auto operator=(CoGenerator&& other) -> CoGenerator& {
        handle = std::exchange(other.handle, nullptr);
        return *this;
    }

    CoGenerator() = default;

    CoGenerator(CoGenerator&) = delete;

    CoGenerator(CoGenerator&& o) : handle(std::exchange(o.handle, nullptr)) {
    }

    CoGenerator(promise_type& promise) : handle(std::coroutine_handle<promise_type>::from_promise(promise)) {}

    ~CoGenerator() {
        if(handle != nullptr) {
            handle.destroy();
        }
    }
};

template <class YieldType>
struct CoRoutine {
    using Generator = CoGenerator<YieldType>;

    Generator generator;

    template <class... Args>
    auto start(auto routine, Args&&... args) -> void {
        generator = routine(std::forward<Args>(args)...);
    }

    auto resume() -> YieldType {
        generator.handle.resume();
        return generator.handle.promise().data;
    }

    auto done() const -> bool {
        return generator.handle.done();
    }
};

#ifdef CUTIL_NS
}
#endif
