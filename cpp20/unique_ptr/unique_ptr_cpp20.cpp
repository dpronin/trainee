#include <functional>
#include <memory>
#include <print>

#include <xroost/memory/unique_ptr.hpp>

int main(int argc, char const *argv[]) {
  xroost::memory::unique_ptr<int> p1{new int{42}};
  static_assert(sizeof(p1) == sizeof(std::addressof(*p1)));

  std::println("{}", sizeof(p1));
  std::println("{}", sizeof(std::addressof(*p1)));

  xroost::memory::unique_ptr<int, std::function<void(int *)>> p2{
      new int{42}, std::default_delete<int>{}};
  static_assert(sizeof(p2) != sizeof(std::addressof(*p2)));

  std::println("{}", sizeof(p2));
  std::println("{}", sizeof(std::addressof(*p2)));

  return 0;
}
