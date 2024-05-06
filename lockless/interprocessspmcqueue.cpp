#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <thread>
#include <type_traits>
#include <vector>

template <typename T>
concept trivial = std::is_trivial_v<T>;

template <trivial T, size_t N> class communication_lockless_spmc_queue {
public:
  communication_lockless_spmc_queue() = default;
  ~communication_lockless_spmc_queue() = default;

  [[nodiscard]] static constexpr uint32_t capacity() noexcept { return N; }

  std::optional<T> pop() noexcept(std::is_nothrow_copy_constructible_v<T> &&
                                  std::is_nothrow_destructible_v<T>) {
    std::optional<T> v;

    auto ph = __atomic_load_n(&head_, __ATOMIC_RELAXED);
    do {
      if (auto const pt = __atomic_load_n(&tail_, __ATOMIC_RELAXED); pt == ph)
          [[unlikely]] {

        v.reset();
        break;
      }
      v = items_[ph];
    } while (!__atomic_compare_exchange_n(&head_, &ph, (ph + 1) % N, true,
                                          __ATOMIC_RELEASE, __ATOMIC_ACQUIRE));

    return v;
  }

  bool push(T const &v) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    auto const ntail = (tail_ + 1) % N;
    if (ntail == __atomic_load_n(&head_, __ATOMIC_RELAXED))
      return false;
    items_[tail_] = v;
    __atomic_store_n(&tail_, ntail, __ATOMIC_RELEASE);
    return true;
  }

private:
  uint32_t head_;
  uint32_t tail_;
  T items_[N];
};

using communication_lockless_spmc_queue_t =
    communication_lockless_spmc_queue<uint32_t, 5u>;

struct process_shared_data {
  communication_lockless_spmc_queue_t q;
};
static_assert(std::is_standard_layout_v<process_shared_data>,
              "process_shared_data must be standard layout");

constexpr size_t kConsumerIterationsCount = 10;

[[noreturn]] void consumer(process_shared_data &info, size_t count) {
  using namespace std::chrono_literals;
  srand(getpid());
  auto &q = info.q;
  for (int i = 0; i < count;) {
    std::cout << getpid() << " (consumer): is working" << std::endl;
    if (auto v = q.pop()) [[likely]] {
      std::cout << getpid() << " (consumer): processing " << *v << std::endl;
      ++i;
    }
    std::cout << getpid() << " (consumer): is sleeping..." << std::endl;
    std::this_thread::sleep_for(200ms +
                                std::chrono::milliseconds(rand() % 101));
  }
  std::cout << getpid() << " (consumer): finished" << std::endl;
  _Exit(EXIT_SUCCESS);
}

void producer(process_shared_data &info, size_t count) {
  using namespace std::chrono_literals;
  srand(getpid());
  auto &q = info.q;
  for (int i = 0; i < count;) {
    std::cout << getpid() << " (producer): is working" << std::endl;
    if (auto const v = /*rand() % 100*/ i; q.push(v)) [[likely]] {
      std::cout << getpid() << " (producer): pushed " << v << std::endl;
      ++i;
    }
    std::cout << getpid() << " (producer): is sleeping..." << std::endl;
    std::this_thread::sleep_for(100ms);
  }
  std::cout << getpid() << " (producer): finished" << std::endl;
}

/* Run example: ./interprocessspmcqueue 10 */
int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cerr << "there must be at least 1 parameter\n";
    return EXIT_FAILURE;
  }

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
  auto process_shared_data_deleter{[](process_shared_data *p_info) {
    p_info->~process_shared_data();
    munmap(p_info, sizeof(*p_info));
  }};
  std::unique_ptr<process_shared_data, decltype(process_shared_data_deleter)>
      p_info{
          new (proc_info_raw) process_shared_data{},
          process_shared_data_deleter,
      };

  std::vector<pid_t> children;

  /*
   * Create up to argv[1] children by forking
   */
  for (auto _ : std::views::iota(0ul, strtoul(argv[1], nullptr, 10))) {
    if (auto const child = fork(); 0 == child) {
      /* We're in a child's body, start doing concurrent fetching from the queue
       */
      children.clear();
      consumer(*p_info, kConsumerIterationsCount);
    } else if (child > 0) {
      /* We're in a parent's body, remember a new child's PID */
      children.push_back(child);
    } else {
      assert(0);
    }
  }

  /* Produce data and push into the queue */
  producer(*p_info, children.size() * kConsumerIterationsCount);

  /* While there are children we would wait for them all to exit */
  while (!children.empty()) {
    int wstatus{0};
    auto const child = waitpid(-1, &wstatus, 0);
    if (WIFEXITED(wstatus))
      std::cout << "child " << child << " exited" << std::endl;
    std::erase_if(children, [=](auto pid) { return pid == child; });
  }

  return EXIT_SUCCESS;
}
