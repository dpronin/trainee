#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <new>
#include <optional>
#include <thread>
#include <type_traits>

template <typename T>
concept standard_layout = std::is_standard_layout_v<T>;

#ifdef __cpp_lib_hardware_interference_size
constexpr size_t hardware_destructive_interference_size{
    std::hardware_destructive_interference_size};
#else
constexpr size_t hardware_destructive_interference_size{64};
#endif

template <standard_layout T, size_t N> class communication_lockless_spsc_queue {
public:
  communication_lockless_spsc_queue() = default;
  ~communication_lockless_spsc_queue() = default;

  [[nodiscard]] static constexpr uint32_t capacity() noexcept { return N; }

  std::optional<T> pop() noexcept(std::is_nothrow_copy_constructible_v<T> &&
                                  std::is_nothrow_destructible_v<T>) {
    std::optional<T> v;

    auto const ch{head_.load(std::memory_order_relaxed)};
    if (auto const ct{tail_.load(std::memory_order_acquire)}; ct != ch)
        [[likely]] {

      v = items_[ch];
      head_.store((ch + 1) % items_.size(), std::memory_order_release);
    }

    return v;
  }

  bool push(T const &v) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    auto const ct{tail_.load(std::memory_order_relaxed)};

    auto const nt{(ct + 1) % items_.size()};
    if (nt == head_.load(std::memory_order_relaxed)) [[unlikely]]
      return false;

    items_[ct] = v;
    tail_.store(nt, std::memory_order_release);

    return true;
  }

private:
  alignas(hardware_destructive_interference_size) std::atomic_uint32_t head_;
  alignas(hardware_destructive_interference_size) std::atomic_uint32_t tail_;
  alignas(hardware_destructive_interference_size) std::array<T, N + 1> items_;
};

using communication_lockless_spsc_queue_t =
    communication_lockless_spsc_queue<std::pair<int, int>, 5u>;

struct process_shared_data {
  communication_lockless_spsc_queue_t q;
};
static_assert(standard_layout<process_shared_data>,
              "process_shared_data must be standard layout");

constexpr size_t kConsumerIterationsCount{100};

[[noreturn]] void consumer(process_shared_data &info, size_t count) {
  using std::chrono_literals::operator""ms;

  auto logln{
      [](auto &&...args) {
        ((std::cout << std::format("{} (consumer): ", getpid())) << ... << args)
            << std::endl;
      },
  };

  srand(getpid());

  for (auto &q{info.q}; count > 0;) {
    logln("working...");

    if (auto v = q.pop()) [[likely]] {
      logln("popped v[", v->first, "] = ", v->second);
      --count;
    }

    auto const sleep_time_ms{100ms + std::chrono::milliseconds(rand() % 51)};
    logln("sleeping for ", sleep_time_ms.count(), "ms ...");
    std::this_thread::sleep_for(sleep_time_ms);
  }

  logln("finished");

  _Exit(EXIT_SUCCESS);
}

void producer(process_shared_data &info, size_t count) {
  using std::chrono_literals::operator""ms;

  auto logln{
      [](auto &&...args) {
        ((std::cout << std::format("{} (producer): ", getpid())) << ... << args)
            << std::endl;
      },
  };

  srand(getpid());

  auto &q{info.q};
  for (int i{0}; i < count;) {
    logln("working...");

    if (auto const v{rand() % 100}; q.push({i, v})) [[likely]] {
      logln("pushed v[", i, "] = ", v);
      ++i;
    }

    auto const sleep_time_ms{20ms};
    logln("sleeping for ", sleep_time_ms.count(), "ms ...");
    std::this_thread::sleep_for(sleep_time_ms);
  }

  logln("finished");
}

/* Run example: ./interprocessspscqueue */
int main(int argc, char const *argv[]) {
  /*
   * Create a memory mapping where shared data among parent and children
   * processes would be located
   */
  auto *proc_info_raw{
      mmap(nullptr, sizeof(process_shared_data), PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, -1, 0),
  };
  assert(proc_info_raw != MAP_FAILED);

  /*
   * Protect the mapping by unique pointer to gracefully unmap the region when
   * exiting the scope for any reason
   */
  auto process_shared_data_deleter{
      [](process_shared_data *p_info) {
        p_info->~process_shared_data();
        munmap(p_info, sizeof(*p_info));
      },
  };
  std::unique_ptr<process_shared_data, decltype(process_shared_data_deleter)>
      p_info{
          new (proc_info_raw) process_shared_data{},
          process_shared_data_deleter,
      };

  auto const child = fork();
  if (0 == child) {
    /*
     * We're in a child's body, start doing concurrent fetching values from
     * the queue
     */
    consumer(*p_info, kConsumerIterationsCount);
  }

  assert(child > 0);

  /* Produce data and push into the queue */
  producer(*p_info, kConsumerIterationsCount);

  /* While there are children we would wait for them all to exit */
  int wstatus{0};
  auto const pid{waitpid(child, &wstatus, 0)};
  assert(pid == child);
  if (WIFEXITED(wstatus))
    std::cout << std::format("child {} exited", child) << std::endl;

  return EXIT_SUCCESS;
}
