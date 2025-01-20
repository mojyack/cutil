#include <print>

#include "coroutine.hpp"

using Worker = CoRoutine<int>;

auto worker_main(int& arg) -> Worker::Generator {
    auto count = 0;
    while(arg != -1) {
        count += arg;
        co_yield 0;
    }
    co_return count;
}

auto main() -> int {
    auto worker = Worker();
    auto arg    = -1;
    auto ret    = -1;
    worker.start(worker_main, arg);
    for(auto i : {1, 2, 3, 4, 5, -1}) {
        arg = i;
        ret = worker.resume();
        std::println("arg={} ret={}", arg, ret);
        if(worker.done()) {
            std::println("done");
            break;
        }
    }
    if(ret != 15) {
        return 1;
    }
    return 0;
}
