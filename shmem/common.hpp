#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <string_view>
#include <utility>

inline constexpr size_t kMaxMsgSize{256};

inline std::shared_ptr<void> shmem_open(std::string_view path, mode_t mode,
                                        int memfl, size_t mem_sz,
                                        bool extend = false) {
  auto shm_fd_closer{[](int const *pfd) {
    close(*pfd);
    delete pfd;
  }};
  auto shm_unmapper{[=](void *p) { munmap(p, mem_sz); }};

  auto const shm_fd{shm_open(path.data(), mode, S_IRUSR | S_IWUSR)};
  if (shm_fd < 0)
    return {};

  std::unique_ptr<const int, decltype(shm_fd_closer)> shm_guard{
      new int{shm_fd},
      shm_fd_closer,
  };

  if (extend && ftruncate(shm_fd, mem_sz) < 0)
    return {};

  auto *p_mem{mmap(nullptr, mem_sz, memfl, MAP_SHARED, shm_fd, 0)};
  if (MAP_FAILED == p_mem)
    return {};

  return std::shared_ptr<void>{
      p_mem,
      [=, shmg = std::move(shm_guard)](void *p) { shm_unmapper(p); },
  };
}
