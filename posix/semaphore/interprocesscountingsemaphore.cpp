#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <ranges>
#include <thread>
#include <type_traits>

template <typename T>
concept trivial = std::is_trivial_v<T>;

template <trivial T, size_t N> class communication_queue {
public:
  communication_queue() = default;
  ~communication_queue() = default;

  [[nodiscard]] static constexpr uint32_t capacity() noexcept { return N; }

  [[nodiscard]] T const &front() const noexcept { return item[head_]; }

  void pop() noexcept {
    head_ = (head_ + 1) % N;
    --size_;
  }

  void push(T const &v) noexcept {
    item[tail_] = v;
    tail_ = (tail_ + 1) % N;
    ++size_;
  }

  [[nodiscard]] uint32_t head() const noexcept { return head_; }
  [[nodiscard]] uint32_t tail() const noexcept { return tail_; }
  [[nodiscard]] uint32_t size() const noexcept { return size_; }

  [[nodiscard]] bool empty() const noexcept { return 0 == size(); }
  [[nodiscard]] bool full() const noexcept { return capacity() == size(); }

private:
  uint32_t head_;
  uint32_t tail_;
  uint32_t size_;
  T item[N];
};

using communication_queue_t = communication_queue<uint32_t, 5u>;

struct process_shared_data {
  sem_t empty;
  sem_t full;
  sem_t mutex;
  communication_queue_t q;
};
static_assert(std::is_standard_layout_v<process_shared_data>,
              "process_shared_data must be standard layout");

constexpr size_t kConsumerIterationsCount = 10;

[[noreturn]] void consumer(process_shared_data &info, size_t count) {
  using namespace std::chrono_literals;
  srand(getpid());
  auto &q = info.q;
  for (auto _ : std::views::iota(static_cast<size_t>(0), count)) {
    sem_wait(&info.empty);
    sem_wait(&info.mutex);
    std::cout << getpid() << " (consumer): is working" << std::endl;
    assert(!q.empty());
    std::cout << getpid() << " (consumer): processing q[" << q.head()
              << "] = " << q.front() << std::endl;
    q.pop();
    std::cout << getpid() << " (consumer): is sleeping..." << std::endl;
    sem_post(&info.mutex);
    sem_post(&info.full);
    std::this_thread::sleep_for(200ms +
                                std::chrono::milliseconds(rand() % 101));
  }
  _Exit(EXIT_SUCCESS);
}

void producer(process_shared_data &info, size_t count) {
  using namespace std::chrono_literals;
  srand(getpid());
  auto &q = info.q;
  for (auto _ : std::views::iota(static_cast<size_t>(0), count)) {
    sem_wait(&info.full);
    sem_wait(&info.mutex);
    std::cout << getpid() << " (producer): is working" << std::endl;
    assert(!q.full());
    auto const v = rand() % 100;
    std::cout << getpid() << " (producer): pushing " << v << " at q["
              << q.tail() << "]" << std::endl;
    q.push(v);
    std::cout << getpid() << " (producer): is sleeping..." << std::endl;
    sem_post(&info.mutex);
    sem_post(&info.empty);
    std::this_thread::sleep_for(100ms);
  }
}

/* Run example: ./interprocesscountingsemaphore 10 */
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

  /* All semaphores must be initialized */
  if (sem_init(&p_info->empty, 1, 0) < 0 ||
      sem_init(&p_info->full, 1, communication_queue_t::capacity()) < 0 ||
      sem_init(&p_info->mutex, 1, 1) < 0) {
    assert(0);
  }

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

  /* All semaphores must be destroyed */
  sem_destroy(&p_info->mutex);
  sem_destroy(&p_info->full);
  sem_destroy(&p_info->empty);

  return EXIT_SUCCESS;
}
