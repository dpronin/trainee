#include <cassert>
#include <cstddef>

#include <iostream>
#include <mutex>
#include <new>
#include <numeric>
#include <ranges>
#include <semaphore>
#include <stack>
#include <thread>
#include <vector>

constexpr auto kConcurrencyLevelMax{5u};
constexpr auto kPerThreadCount{100000000u};

template <typename T, size_t Align>
#ifdef __clang__
struct aligned_storage_impl {
  alignas(Align) std::byte data[sizeof(T)];
};
#else
using aligned_storage_impl = alignas(Align) std::byte[sizeof(T)];
#endif

template <typename T, size_t Align = alignof(T)> struct aligned_storage {
  static_assert(!(Align < alignof(T)));
  using type = aligned_storage_impl<T, Align>;
};

template <typename T, size_t Align = alignof(T)>
using aligned_storage_t = typename aligned_storage<T, Align>::type;

#ifdef __cpp_lib_hardware_interference_size
constexpr size_t hardware_constructive_interference_size =
    std::hardware_constructive_interference_size;
#else
constexpr size_t hardware_constructive_interference_size = 64;
#endif

using int_cache_line_aligned =
    aligned_storage_t<int, hardware_constructive_interference_size>;

struct thread_shared_info {
  thread_shared_info() {
    for (auto &i : v)
      items_pool.push(&i);
  }

  std::counting_semaphore<kConcurrencyLevelMax> sem{kConcurrencyLevelMax};
  std::vector<int_cache_line_aligned> v{kConcurrencyLevelMax};

  std::mutex items_pool_lock;
  std::stack<typename decltype(v)::pointer> items_pool;
};

void worker(thread_shared_info &shared_data) {
  shared_data.sem.acquire();

  std::unique_lock l{shared_data.items_pool_lock};

  std::cout << "Thread started ..." << std::endl;

  assert(!shared_data.items_pool.empty());

  auto *p{shared_data.items_pool.top()};
  shared_data.items_pool.pop();

  l.unlock();

  for (auto &i{reinterpret_cast<int &>(*p)};
       auto _ : std::views::iota(0u, kPerThreadCount)) {
    ++i;
  }

  l.lock();

  shared_data.items_pool.push(p);

  std::cout << "Thread finished ..." << std::endl;

  l.unlock();

  shared_data.sem.release();
}

int main(int argc, char const *argv[]) {
  auto shared_data{thread_shared_info{}};

  auto vths{std::vector<std::thread>{std::thread::hardware_concurrency()}};
  for (auto &vth : vths)
    vth = std::thread{worker, std::ref(shared_data)};

  for (auto &vth : vths)
    vth.join();

  auto vi{
      shared_data.v | std::views::transform([](auto &&x) -> int {
        return reinterpret_cast<int const &>(std::forward<decltype(x)>(x));
      }),
  };

  auto const r{std::reduce(vi.begin(), vi.end())};

  std::cout << r << std::endl;

  assert(r == kPerThreadCount * vths.size());

  return 0;
}
