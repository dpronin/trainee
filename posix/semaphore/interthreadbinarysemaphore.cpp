#include <cassert>

#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

#include <semaphore.h>

constexpr auto kPerThreadCount{100000000u};

struct thread_shared_info {
  sem_t sem;
  int cnt;
};

void f(thread_shared_info &info) {
  sem_wait(&info.sem);
  std::cout << "Thread started ..." << std::endl;
  for (auto _ : std::views::iota(0u, kPerThreadCount))
    ++info.cnt;
  std::cout << "Thread finished ..." << std::endl;
  sem_post(&info.sem);
}

int main(int argc, char const *argv[]) {
  auto shared_data{thread_shared_info{}};

  [[maybe_unused]] auto const r{sem_init(&shared_data.sem, 0, 1)};
  assert(0 == r);

  auto vths{std::vector<std::thread>{std::thread::hardware_concurrency()}};
  for (auto &vth : vths)
    vth = std::thread{f, std::ref(shared_data)};

  for (auto &vth : vths)
    vth.join();

  sem_destroy(&shared_data.sem);

  std::cout << shared_data.cnt << std::endl;

  assert(shared_data.cnt == kPerThreadCount * vths.size());

  return 0;
}
