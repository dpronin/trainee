#include <cerrno>
#include <cstdlib>

#include <algorithm>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <ranges>
#include <stdexcept>
#include <system_error>
#include <typeinfo>
#include <vector>

#ifdef __clang__
#include <print>
#else
#include <format>
#endif

#include <unistd.h>

#ifndef __clang__

namespace std {

template <typename... Args>
void println(std::ostream &os, std::format_string<Args...> fmt,
             Args &&...args) {
  os << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

} // namespace std

#endif

template <typename T> class Allocator final {
public:
  using value_type = T;

  explicit Allocator(size_t alignment = alignof(T)) : alignment_(alignment) {
    if ((alignment_ - 1) & alignment_)
      throw std::invalid_argument(
          std::format("{} must be a power of 2", alignment_));
    if (alignment_ < alignof(T))
      throw std::invalid_argument(
          std::format("{} must be at least {}", alignment_, alignof(T)));
  }
  ~Allocator() = default;

  template <typename U>
  constexpr explicit Allocator(Allocator<U> const &other)
      : alignment_(std::max(other.alignment(), alignof(U))) {}

  Allocator(Allocator const &other) = default;
  Allocator(Allocator &&other) = default;

  size_t alignment() const { return alignment_; }

  [[nodiscard]] T *allocate(size_t n) {
    std::println(
        std::cout,
        "allocate called for {}, required {} bytes {} times, alignment {:#x}",
        typeid(T).name(), sizeof(T), n, alignment_);

    if (n > std::numeric_limits<size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();

    if (auto *p = static_cast<T *>(aligned_alloc(alignment_, sizeof(T) * n)))
      return p;

    throw std::bad_alloc();
  }

  void deallocate(T *p, size_t n) noexcept { std::free(p); }

private:
  size_t alignment_;
};

int main(int argc, char const *argv[]) try {
  std::vector<std::shared_ptr<int>> v;

  for (auto a1{Allocator<int>{1 << 28}};
       auto const i : std::views::iota(0, 1 << 4)) {
    v.push_back(std::allocate_shared<int>(a1, i));
    std::println(std::cout, "{}", static_cast<void *>(v.back().get()));
  }

  v.clear();

  auto const page_size{getpagesize()};
  if (page_size < 0)
    throw std::system_error{errno, std::generic_category()};

  for (auto a2{Allocator<int>{static_cast<size_t>(page_size)}};
       auto const i : std::views::iota(0, 1 << 4)) {
    v.push_back(std::allocate_shared<int>(a2, i));
    std::println(std::cout, "{}", static_cast<void *>(v.back().get()));
  }

  return 0;
} catch (std::exception const &ex) {
  std::println(std::cerr, "{}", ex.what());
  return 1;
} catch (...) {
  std::println(std::cerr, "unknown exception occurred");
  return 1;
}
