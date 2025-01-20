#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <memory>
#include <print>

#include <xroost/utility/aligned_storage.hpp>

class IShared {
public:
  IShared() = default;
  virtual ~IShared() = default;

  IShared(IShared const &) = delete;
  IShared &operator=(IShared const &) = delete;

  IShared(IShared &&) = delete;
  IShared &operator=(IShared &&) = delete;

  virtual void print() = 0;
};

class Shared final : public IShared {
public:
  explicit Shared(int x) : x_{x} {}
  ~Shared() override = default;

  Shared(Shared const &) = delete;
  Shared &operator=(Shared const &) = delete;

  Shared(Shared &&) = delete;
  Shared &operator=(Shared &&) = delete;

  void print() override {
    std::println("x is {}, this is {:#x}, getpid() {}", x_,
                 reinterpret_cast<uintptr_t>(this), getpid());
  }

private:
  int x_;
};

template <typename T> struct placement_deleter {
  void operator()(T *ptr) { std::destroy_at(ptr); }
};

struct process_shared_data {
  explicit process_shared_data(int x) : ptr{new(&storage) Shared{x}} {}
  ~process_shared_data() = default;

  process_shared_data(process_shared_data const &) = delete;
  process_shared_data &operator=(process_shared_data const &) = delete;

  process_shared_data(process_shared_data &&) = delete;
  process_shared_data &operator=(process_shared_data &&) = delete;

  xroost::aligned_storage_t<Shared> storage;
  std::unique_ptr<IShared, placement_deleter<IShared>> ptr;
};

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
          new (proc_info_raw) process_shared_data{42},
          process_shared_data_deleter,
      };

  auto const child = fork();
  if (0 == child) {
    p_info->ptr->print();
    _Exit(EXIT_SUCCESS);
  }

  assert(child > 0);

  p_info->ptr->print();

  /* While there are children we would wait for them all to exit */
  int wstatus{0};
  auto const pid{waitpid(child, &wstatus, 0)};
  assert(pid == child);
  if (WIFEXITED(wstatus))
    std::println("child {} exited", child);

  return EXIT_SUCCESS;
}
