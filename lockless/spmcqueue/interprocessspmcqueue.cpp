#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <ranges>
#include <thread>
#include <type_traits>
#include <vector>

#include <xroost/lockless/spmcqueue.hpp>

template <typename T>
concept standard_layout = std::is_standard_layout_v<T>;

using queue_t = static_spmcqueue<std::pair<int, int>, 5u>;

struct process_shared_data {
  queue_t q;
};
static_assert(standard_layout<process_shared_data>,
              "process_shared_data must be standard layout");

constexpr size_t kConsumerIterationsCount{10};

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

  std::vector<pid_t> children;

  /*
   * Create up to argv[1] children by forking
   */
  for (auto _ : std::views::iota(0ul, strtoul(argv[1], nullptr, 10))) {
    if (auto const child = fork(); 0 == child) {
      /*
       * We're in a child's body, start doing concurrent fetching values from
       * the queue
       */
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
    auto const child{waitpid(-1, &wstatus, 0)};
    if (WIFEXITED(wstatus))
      std::cout << std::format("child {} exited", child) << std::endl;
    std::erase_if(children, [=](auto pid) { return pid == child; });
  }

  return EXIT_SUCCESS;
}
