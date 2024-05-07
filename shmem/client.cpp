#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <memory>

#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

#include <sys/mman.h>

#include "common.hpp"

/* run ./client /server /client */
int main(int argc, char const *argv[]) {
  if (argc < 3)
    return EXIT_FAILURE;

  auto to_srv_mem{
      shmem_open(argv[1], O_RDWR | O_CREAT, PROT_WRITE | PROT_READ,
                 sizeof(sem_t) + kMaxMsgSize, true),
  };
  if (!to_srv_mem)
    return EXIT_FAILURE;

  auto from_srv_mem{
      shmem_open(argv[2], O_RDWR | O_CREAT, PROT_WRITE | PROT_READ,
                 sizeof(sem_t) + kMaxMsgSize),
  };
  if (!from_srv_mem)
    return EXIT_FAILURE;

  auto *p_to_srv_sem{static_cast<sem_t *>(to_srv_mem.get())};
  if (sem_init(p_to_srv_sem, 1, 0) < 0)
    return EXIT_FAILURE;

  auto sem_closer{[](sem_t *sem) { sem_close(sem); }};
  auto to_srv_sem{
      std::unique_ptr<sem_t, decltype(sem_closer)>{p_to_srv_sem, sem_closer},
  };

  auto *p_to_srv_msg{static_cast<char *>(to_srv_mem.get()) + sizeof(sem_t)};
  auto *p_from_srv_sem{static_cast<sem_t *>(from_srv_mem.get())};
  auto *p_from_srv_msg{static_cast<char *>(from_srv_mem.get()) + sizeof(sem_t)};

  while (true) {
    std::fputs("> ", stdout);
    std::fgets(p_to_srv_msg, kMaxMsgSize, stdin);
    p_to_srv_msg[kMaxMsgSize] = '\0';
    sem_post(to_srv_sem.get());

    sem_wait(p_from_srv_sem);
    p_from_srv_msg[kMaxMsgSize] = '\0';
    std::cout << p_from_srv_msg << std::endl;
  }

  return 0;
}
