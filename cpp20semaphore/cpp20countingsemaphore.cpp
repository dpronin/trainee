#include <cassert>

#include <atomic>
#include <semaphore>
#include <thread>
#include <iostream>
#include <vector>

constexpr auto kConcurrencyLevelMax{5u};
constexpr auto kPerThreadCount{1000000u};

struct thread_shared_info {
  std::counting_semaphore<kConcurrencyLevelMax> sem{kConcurrencyLevelMax};
  std::atomic_int32_t a{0};
  std::atomic_int32_t cnt{0};
};

void f(thread_shared_info &info) {
  info.sem.acquire();

  auto p {info.a.fetch_add(1, std::memory_order_relaxed)};
  assert(p < kConcurrencyLevelMax);

  for (int i = 0; i < kPerThreadCount; ++i)
    info.cnt.fetch_add(1, std::memory_order_relaxed);

  p = info.a.fetch_sub(1, std::memory_order_relaxed);
  assert(p > 0);

  info.sem.release();
}

int main(int argc, char const *argv[]) {
  auto shared_data{thread_shared_info{}};

  auto vths{std::vector<std::thread>{std::thread::hardware_concurrency()}};
  for (auto &vth : vths)
    vth = std::thread{f, std::ref(shared_data)};

  for (auto &vth : vths)
    vth.join();

  auto const r{shared_data.cnt.load(std::memory_order_relaxed)};

  std::cout << r << std::endl;

  assert(r == kPerThreadCount * vths.size());

  return 0;
}
