#include <cassert>
#include <cstddef>

#include <iostream>
#include <limits>
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
constexpr size_t hardware_destructive_interference_size =
    std::hardware_destructive_interference_size;
#else
constexpr size_t hardware_destructive_interference_size = 64;
#endif

using int_cache_line_aligned =
    aligned_storage_t<int, hardware_destructive_interference_size>;

template <ptrdiff_t LeastMaxValue = std::numeric_limits<ptrdiff_t>::max()>
class semaphore {
  std::counting_semaphore<LeastMaxValue> s_{LeastMaxValue};

public:
  semaphore() = default;
  semaphore(semaphore const &) = delete;

  void lock() { s_.acquire(); }
  bool try_lock() { return s_.try_acquire(); }
  void unlock() { s_.release(); }
};

struct thread_shared_info {
  thread_shared_info() {
    for (auto &i : v)
      items_pool.push(&reinterpret_cast<int &>(i));
  }

  semaphore<kConcurrencyLevelMax> sem;
  std::vector<int_cache_line_aligned> v{kConcurrencyLevelMax};

  std::mutex items_pool_lock;
  std::stack<int *> items_pool;
};

void worker(thread_shared_info &shared_data) {
  std::lock_guard lgs{shared_data.sem};

  std::unique_lock ulm{shared_data.items_pool_lock};

  std::cout << "Thread started ..." << std::endl;

  assert(!shared_data.items_pool.empty());

  auto *p_cnt{shared_data.items_pool.top()};
  shared_data.items_pool.pop();

  ulm.unlock();

  for (auto _ : std::views::iota(0u, kPerThreadCount))
    ++(*p_cnt);

  ulm.lock();

  shared_data.items_pool.push(p_cnt);

  std::cout << "Thread finished ..." << std::endl;

  ulm.unlock();
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
