#include <cassert>

#include <semaphore>
#include <thread>
#include <iostream>
#include <vector>

constexpr auto kPerThreadCount{100000000u};

struct thread_shared_info {
  std::binary_semaphore sem{1};
  int cnt;
};

void f(thread_shared_info &info) {
  info.sem.acquire();
  std::cout << "Thread started ..." << std::endl;
  for (int i = 0; i < kPerThreadCount; ++i)
    ++info.cnt;
  std::cout << "Thread finished ..." << std::endl;
  info.sem.release();
}

int main(int argc, char const *argv[]) {
  auto shared_data{thread_shared_info{}};

  auto vths{std::vector<std::thread>{std::thread::hardware_concurrency()}};
  for (auto &vth : vths)
    vth = std::thread{f, std::ref(shared_data)};

  for (auto &vth : vths)
    vth.join();

  std::cout << shared_data.cnt << std::endl;

  assert(shared_data.cnt == kPerThreadCount * vths.size());

  return 0;
}
