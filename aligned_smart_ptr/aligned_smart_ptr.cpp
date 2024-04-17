#include <cstdlib>

#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <print>
#include <ranges>
#include <typeinfo>
#include <vector>

#include <unistd.h>

template <typename T, size_t Alignment = alignof(T)> class Allocator {
public:
  static_assert(0 == (Alignment & (Alignment - 1)),
                "Alignment must be a power of 2");
  static_assert(alignof(T) <= Alignment,
                "Alignment must be at least alignof(T)");
  template <class U> struct rebind {
    using other = Allocator<U, Alignment /*std::max(alignof(U), Alignment)*/>;
  };

  using value_type = T;

  Allocator() = default;
  ~Allocator() = default;

  template <typename U, size_t A>
  constexpr explicit Allocator(Allocator<U, A> const &other) noexcept {}

  Allocator(Allocator const &other) = default;
  Allocator(Allocator &&other) = default;

  T *allocate(std::size_t n) {
    std::println("allocate called for {}, required {} bytes {} times",
                 typeid(T).name(), sizeof(T), n);

    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();

    if (auto *p = static_cast<T *>(aligned_alloc(Alignment, sizeof(T) * n)))
      return p;

    throw std::bad_alloc();
  }

  void deallocate(T *p, std::size_t n) noexcept { std::free(p); }
};

template <typename T = void> class PageAlignedAllocator {
public:
  template <typename U> struct rebind {
    using other = PageAlignedAllocator<U>;
  };

  using value_type = T;

  constexpr PageAlignedAllocator() = default;
  ~PageAlignedAllocator() = default;

  template <typename U>
  constexpr explicit PageAlignedAllocator(
      PageAlignedAllocator<U> const &other) noexcept {}

  PageAlignedAllocator(PageAlignedAllocator<T> const &other) = default;
  PageAlignedAllocator(PageAlignedAllocator<T> &&other) = default;

  T *allocate(std::size_t n) {
    std::println("allocate called for {}, required {} bytes {} times",
                 typeid(T).name(), sizeof(T), n);

    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();

    if (auto const page_sz = getpagesize(); page_sz > 0) {
      if (auto *p =
              static_cast<T *>(std::aligned_alloc(page_sz, sizeof(T) * n)))
        return p;
    }

    throw std::bad_alloc();
  }

  void deallocate(T *p, std::size_t n) noexcept { std::free(p); }
};

int main(int argc, char const *argv[]) try {
  std::vector<std::shared_ptr<int>> v;

  Allocator<int, 1 << 28> a1{};
  for (auto const i : std::views::iota(0, 1 << 4)) {
    v.push_back(std::allocate_shared<int>(a1, i));
    std::println("{}", static_cast<void *>(v.back().get()));
  }

  v.clear();

  PageAlignedAllocator<> a2{};
  for (auto const i : std::views::iota(0, 1 << 4)) {
    v.push_back(std::allocate_shared<int>(a2, i));
    std::println("{}", static_cast<void *>(v.back().get()));
  }

  return 0;
} catch (std::exception const &ex) {
  std::cerr << ex.what() << std::endl;
  return 1;
}
