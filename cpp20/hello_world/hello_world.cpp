#ifdef __clang__
#include <print>
#else
#include <format>
#include <iostream>

namespace std {

template <typename... Args>
void println(format_string<Args...> fmt, Args &&...args) {
  cout << format(fmt, forward<Args>(args)...) << endl;
}

} // namespace std

#endif

int main(int argc, char const *argv[]) {
  std::println("Hello, world");
  return 0;
}
