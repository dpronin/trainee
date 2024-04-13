#include <cstdlib>

#include <initializer_list>
#include <filesystem>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>

namespace {

inline pid_t childize() {
  pid_t pid = fork();
  if (pid > 0)
    exit(EXIT_SUCCESS);
  return pid;
}

inline int daemonize() {
  pid_t pid = childize();
  if (pid < 0)
    return -1;

  if (setsid() < 0)
    return -1;

  pid = childize();
  if (pid < 0)
    return -1;

  if (std::error_code ec{}; std::filesystem::current_path("/", ec), ec)
    return -1;

  for (auto fd : {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO})
    close(fd);

  if (STDIN_FILENO != open("/dev/null", 0))
    return -1;

  for (auto fd : {STDOUT_FILENO, STDERR_FILENO}) {
    if (fd != dup(STDIN_FILENO))
      return -1;
  }

  return 0;
}

} // namespace

/* example: run program like ./daemon 10 */

int main(int argc, char const *argv[]) {
  if (argc < 2)
    return EXIT_FAILURE;

  if (daemonize() < 0)
    return EXIT_FAILURE;

  sleep(strtoul(argv[1], NULL, 10));

  return 0;
}
