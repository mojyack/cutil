#include <array>
#include <thread>

#include "print.hpp"
#include "rcu.hpp"

constexpr auto updater_interval = std::chrono::milliseconds(100);
constexpr auto test_duration    = std::chrono::milliseconds(1000);
constexpr auto data_size        = 64;
constexpr auto num_workers      = 8;

struct Data {
    std::array<int, data_size> arr;
};

auto score     = std::atomic_int();
auto running   = true;
auto data      = (Data*)(nullptr);
auto data_lock = std::mutex();
auto rcu_data  = RCU<Data>();
auto updater   = std::thread();
auto workers   = std::array<std::thread, num_workers>();

auto validate_array(std::array<int, data_size> arr) -> void {
    for(auto n : arr) {
        if(n != arr[0]) {
            auto buf = std::string("error! ");
            for(auto n : arr) {
                buf += std::to_string(n) + " ";
            }
            printf("%s\n", buf.data());
            break;
        }
    }
}

auto dangerous_main(bool updater) -> void {
    if(updater) {
        for(auto i = 0; running; i += 1) {
            for(auto& num : data->arr) {
                num = i % 10;
            }
            std::this_thread::sleep_for(updater_interval);
        }
    } else {
        while(running) {
            validate_array(data->arr);
            score.fetch_add(1);
        }
    }
}

auto mutex_main(bool updater) -> void {
    if(updater) {
        for(auto i = 0; running; i += 1) {
            data_lock.lock();
            for(auto& num : data->arr) {
                num = i % 10;
            }
            data_lock.unlock();
            std::this_thread::sleep_for(updater_interval);
        }
    } else {
        while(running) {
            data_lock.lock();
            validate_array(data->arr);
            data_lock.unlock();
            score.fetch_add(1);
        }
    }
}

auto rcu_main(bool updater) -> void {
    if(updater) {
        for(auto i = 1; running; i += 1) {
            auto tmp = rcu_data.copy_data();
            for(auto& num : tmp.arr) {
                num = i % 10;
            }
            const auto spins = rcu_data.emplace(std::move(tmp));
            if(spins != 0) {
                print("spins ", spins);
            }
            std::this_thread::sleep_for(updater_interval);
        }
    } else {
        while(running) {
            auto rcu = rcu_data.lock();
            validate_array(rcu->data.arr);
            rcu->unlock();
            score.fetch_add(1);
        }
    }
}

auto rcu_autolock_main(bool updater) -> void {
    if(updater) {
        rcu_main(true);
    } else {
        while(running) {
            auto [lock, data] = rcu_data.access();
            validate_array(data.arr);
            score.fetch_add(1);
        }
    }
}

auto start_threads(auto main) -> void {
    running = true;
    updater = std::thread(main, true);
    for(auto& worker : workers) {
        worker = std::thread(main, false);
    }
}

auto finish_workers() -> void {
    running = false;
    for(auto& worker : workers) {
        worker.join();
    }
    updater.join();
}

auto run_test(auto main, int baseline) -> int {
    start_threads(main);
    score = 0;
    std::this_thread::sleep_for(test_duration);
    auto record = int(score);
    finish_workers();
    if(baseline == 0) {
        print("score: ", record);
    } else {
        print("score: ", record, " ", 100. * record / baseline, "%");
    }
    return record;
}

auto main() -> int {
    data = new Data();
    rcu_data.emplace();

    print("=== dangerous ===");
    auto dangerous_score = run_test(dangerous_main, 0);

    print("=== mutex ===");
    auto mutex_score = run_test(mutex_main, dangerous_score);

    print("=== rcu ===");
    auto rcu_score = run_test(rcu_main, dangerous_score);

    print("=== rcu(autolock) ===");
    auto rcu_score2 = run_test(rcu_autolock_main, dangerous_score);

    print("=== result ===");
    print("dangerous:     ", dangerous_score, " (", dangerous_score / 1'000'100.0, "M)");
    print("mutex:         ", mutex_score, " (", mutex_score / 1'000'100.0, "M)");
    print("rcu:           ", rcu_score, " (", rcu_score / 1'000'100.0, "M)");
    print("rcu(autolock): ", rcu_score2, " (", rcu_score2 / 1'000'100.0, "M)");

    return 0;
}
