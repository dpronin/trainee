#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <memory>

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include "common.hpp"

/* run ./server /server /client */
int main(int argc, char const *argv[]) {
  if (argc < 3)
    return EXIT_FAILURE;

  auto from_cl_mem{
      shmem_open(argv[1], O_RDWR | O_CREAT, PROT_WRITE | PROT_READ,
                 sizeof(sem_t) + kMaxMsgSize),
  };
  if (!from_cl_mem)
    return EXIT_FAILURE;

  auto to_cl_mem{
      shmem_open(argv[2], O_RDWR | O_CREAT, PROT_WRITE | PROT_READ,
                 sizeof(sem_t) + kMaxMsgSize, true),
  };
  if (!to_cl_mem)
    return EXIT_FAILURE;

  auto *p_to_cl_sem{static_cast<sem_t *>(to_cl_mem.get())};
  if (sem_init(p_to_cl_sem, 1, 0) < 0)
    return EXIT_FAILURE;

  auto sem_closer{[](sem_t *sem) { sem_close(sem); }};
  auto to_cl_sem{
      std::unique_ptr<sem_t, decltype(sem_closer)>{p_to_cl_sem, sem_closer},
  };

  auto *p_to_cl_msg{static_cast<char *>(to_cl_mem.get()) + sizeof(sem_t)};
  auto *p_from_cl_sem{static_cast<sem_t *>(from_cl_mem.get())};
  auto *p_from_cl_msg{static_cast<char *>(from_cl_mem.get()) + sizeof(sem_t)};

  while (true) {
    sem_wait(p_from_cl_sem);
    p_from_cl_msg[kMaxMsgSize] = '\0';
    std::cout << p_from_cl_msg << std::endl;

    std::fputs("> ", stdout);
    std::fgets(p_to_cl_msg, kMaxMsgSize, stdin);
    p_to_cl_msg[kMaxMsgSize] = '\0';
    sem_post(p_to_cl_sem);
  }

  return 0;
}
