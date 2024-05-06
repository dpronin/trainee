#include <cassert>

#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

#include <sys/mman.h>
#include <unistd.h>
#include <wait.h>

#include <semaphore.h>

constexpr auto kPerProcessCount{100000000u};

struct process_shared_info {
  sem_t sem;
  int cnt;
};

[[noreturn]] void worker(process_shared_info &shared_data) {
  sem_wait(&shared_data.sem);
  std::cout << "Process started ..." << std::endl;
  for (auto _ : std::views::iota(0u, kPerProcessCount))
    ++shared_data.cnt;
  std::cout << "Process finished ..." << std::endl;
  sem_post(&shared_data.sem);
  _Exit(0);
}

int main(int argc, char const *argv[]) {
  auto *shared_data_mem{
      mmap(nullptr, sizeof(process_shared_info), PROT_READ | PROT_WRITE,
           MAP_ANONYMOUS | MAP_SHARED, -1, 0),
  };
  assert(shared_data_mem != MAP_FAILED);

  auto *shared_data{new (shared_data_mem) process_shared_info{}};

  [[maybe_unused]] auto const r{sem_init(&shared_data->sem, 1, 1)};
  assert(0 == r);

  auto children{std::vector<pid_t>(std::thread::hardware_concurrency())};
  for (auto &pid : children) {
    pid = fork();
    if (pid == 0) {
      worker(*shared_data);
    } else if (pid < 0) {
      break;
    }
  }

  for (auto child : children)
    waitpid(child, nullptr, 0);

  sem_destroy(&shared_data->sem);

  std::cout << shared_data->cnt << std::endl;

  assert(shared_data->cnt == kPerProcessCount * children.size());

  shared_data->~process_shared_info();

  munmap(shared_data_mem, sizeof(process_shared_info));

  return 0;
}
